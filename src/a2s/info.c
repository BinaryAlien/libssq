#include "ssq/a2s/info.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "cursor.h"
#include "packet.h"
#include "query.h"
#include "response.h"
#include "server.h"

#define SSQ_A2S_INFO_HEADER_REQUEST 0x54
#define SSQ_A2S_INFO_HEADER_RESPONSE 0x49

#define SSQ_A2S_INFO_EDF_GAMEID 0x01
#define SSQ_A2S_INFO_EDF_KEYWORDS 0x20
#define SSQ_A2S_INFO_EDF_PORT 0x80
#define SSQ_A2S_INFO_EDF_SOURCE_TV 0x40
#define SSQ_A2S_INFO_EDF_STEAMID 0x10

static size_t ssq_a2s_info_query_initialize(void *const datagram,
                                            const int32_t challenge) {
  struct ssq_cursor cursor;
  ssq_cursor_initialize(&cursor, datagram, SSQ_PACKET_SIZE);
  ssq_cursor_write_i32(&cursor, SSQ_PACKET_HEADER_SIMPLE);
  ssq_cursor_write_u8(&cursor, SSQ_A2S_INFO_HEADER_REQUEST);
  ssq_cursor_write_string(&cursor, "Source Engine Query");
  ssq_cursor_write(&cursor, &challenge, sizeof(challenge));
  return ssq_cursor_position(&cursor);
}

static struct ssq_buffer *ssq_a2s_info_query(struct ssq_server *const server,
                                             int32_t challenge) {
  uint8_t datagram[SSQ_PACKET_SIZE];
  const size_t datagram_size =
      ssq_a2s_info_query_initialize(datagram, challenge);
  struct ssq_buffer *const response =
      ssq_query(server, datagram, datagram_size);
  if (response == NULL || !ssq_response_has_challenge(response))
    return response;
  challenge = ssq_response_get_challenge(response);
  ssq_buffer_free(response);
  return ssq_a2s_info_query(server, challenge);
}

static enum ssq_a2s_info_environment
ssq_a2s_info_read_environment(struct ssq_cursor *const cursor) {
  switch (ssq_cursor_read_u8(cursor)) {
  case 'l':
    return SSQ_A2S_INFO_ENVIRONMENT_LINUX;
  case 'm':
  case 'o':
    return SSQ_A2S_INFO_ENVIRONMENT_MAC;
  case 'w':
    return SSQ_A2S_INFO_ENVIRONMENT_WINDOWS;
  default:
    return SSQ_A2S_INFO_ENVIRONMENT_UNKNOWN;
  }
}

static enum ssq_a2s_info_server_type
ssq_a2s_info_read_server_type(struct ssq_cursor *const cursor) {
  switch (ssq_cursor_read_u8(cursor)) {
  case 'd':
    return SSQ_A2S_INFO_SERVER_TYPE_DEDICATED;
  case 'l':
    return SSQ_A2S_INFO_SERVER_TYPE_NON_DEDICATED;
  case 'p':
    return SSQ_A2S_INFO_SERVER_TYPE_SOURCE_TV_RELAY;
  default:
    return SSQ_A2S_INFO_SERVER_TYPE_UNKNOWN;
  }
}

struct ssq_a2s_info *
ssq_a2s_info_deserialize(const struct ssq_buffer *const response,
                         struct ssq_error *const error) {
  struct ssq_cursor cursor;
  ssq_cursor_initialize(&cursor, response->data, response->size);
  if (ssq_response_is_truncated(response))
    ssq_cursor_forward(&cursor, sizeof(int32_t));
  const uint8_t header = ssq_cursor_read_u8(&cursor);
  if (header != SSQ_A2S_INFO_HEADER_RESPONSE) {
    ssq_error_set(error, SSQE_BAD, "Bad A2S_INFO response header");
    return NULL;
  }
  struct ssq_a2s_info *const info = malloc(sizeof(*info));
  if (info == NULL) {
    ssq_error_set_from_errno(error);
    return NULL;
  }
  memset(info, 0, sizeof(*info));
  info->protocol = ssq_cursor_read_u8(&cursor);
  info->name = ssq_cursor_read_string(&cursor, &info->name_size);
  info->map = ssq_cursor_read_string(&cursor, &info->map_size);
  info->folder = ssq_cursor_read_string(&cursor, &info->folder_size);
  info->game = ssq_cursor_read_string(&cursor, &info->game_size);
  info->id = ssq_cursor_read_u16(&cursor);
  info->players = ssq_cursor_read_u8(&cursor);
  info->max_players = ssq_cursor_read_u8(&cursor);
  info->bots = ssq_cursor_read_u8(&cursor);
  info->server_type = ssq_a2s_info_read_server_type(&cursor);
  info->environment = ssq_a2s_info_read_environment(&cursor);
  info->password = ssq_cursor_read_bool(&cursor);
  info->secure = ssq_cursor_read_bool(&cursor);
  info->version = ssq_cursor_read_string(&cursor, &info->version_size);
  if (ssq_cursor_available(&cursor) == 0)
    return info;
  info->edf = ssq_cursor_read_u8(&cursor);
  if (ssq_a2s_info_has_port(info))
    info->port = ssq_cursor_read_u16(&cursor);
  if (ssq_a2s_info_has_steamid(info))
    info->steamid = ssq_cursor_read_u64(&cursor);
  if (ssq_a2s_info_has_source_tv(info)) {
    info->source_tv_port = ssq_cursor_read_u16(&cursor);
    info->source_tv_name =
        ssq_cursor_read_string(&cursor, &info->source_tv_name_size);
  }
  if (ssq_a2s_info_has_keywords(info))
    info->keywords = ssq_cursor_read_string(&cursor, &info->keywords_size);
  if (ssq_a2s_info_has_gameid(info))
    info->gameid = ssq_cursor_read_u64(&cursor);
  return info;
}

struct ssq_a2s_info *ssq_a2s_info(struct ssq_server *const server) {
  struct ssq_buffer *const response = ssq_a2s_info_query(server, 0xFFFFFFFF);
  if (response == NULL)
    return NULL;
  struct ssq_a2s_info *const info =
      ssq_a2s_info_deserialize(response, &server->last_error);
  ssq_buffer_free(response);
  return info;
}

void ssq_a2s_info_free(struct ssq_a2s_info *const info) {
  if (info == NULL)
    return;
  free(info->name);
  free(info->map);
  free(info->folder);
  free(info->game);
  free(info->version);
  if (ssq_a2s_info_has_source_tv(info))
    free(info->source_tv_name);
  if (ssq_a2s_info_has_keywords(info))
    free(info->keywords);
  free(info);
}

bool ssq_a2s_info_has_gameid(const struct ssq_a2s_info *const info) {
  return info->edf & SSQ_A2S_INFO_EDF_GAMEID;
}

bool ssq_a2s_info_has_keywords(const struct ssq_a2s_info *const info) {
  return info->edf & SSQ_A2S_INFO_EDF_KEYWORDS;
}

bool ssq_a2s_info_has_port(const struct ssq_a2s_info *const info) {
  return info->edf & SSQ_A2S_INFO_EDF_PORT;
}

bool ssq_a2s_info_has_source_tv(const struct ssq_a2s_info *const info) {
  return info->edf & SSQ_A2S_INFO_EDF_SOURCE_TV;
}

bool ssq_a2s_info_has_steamid(const struct ssq_a2s_info *const info) {
  return info->edf & SSQ_A2S_INFO_EDF_STEAMID;
}
