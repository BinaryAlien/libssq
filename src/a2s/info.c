#include "ssq/a2s/info.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "packet.h"
#include "query.h"
#include "response.h"
#include "server.h"
#include "stream.h"

#define SSQ_REQUEST_HEADER_INFO 0x54
#define SSQ_RESPONSE_HEADER_INFO 0x49

#define SSQ_A2S_INFO_EDF_GAMEID 0x01
#define SSQ_A2S_INFO_EDF_KEYWORDS 0x20
#define SSQ_A2S_INFO_EDF_PORT 0x80
#define SSQ_A2S_INFO_EDF_STEAMID 0x10
#define SSQ_A2S_INFO_EDF_SOURCE_TV 0x40

static struct ssq_response *
ssq_a2s_info_query(struct ssq_server *const server,
                   ssq_response_challenge challenge) {
  uint8_t datagram[SSQ_PACKET_SIZE];
  struct ssq_stream datagram_stream;
  ssq_stream_initialize(&datagram_stream, datagram, sizeof(datagram));
  ssq_stream_write_i32(&datagram_stream, SSQ_PACKET_HEADER_SIMPLE);
  ssq_stream_write_u8(&datagram_stream, SSQ_REQUEST_HEADER_INFO);
  ssq_stream_write_string(&datagram_stream, "Source Engine Query");
  ssq_stream_write(&datagram_stream, &challenge, sizeof(challenge));
  const size_t datagram_size = ssq_stream_position(&datagram_stream);
  struct ssq_response *const response =
      ssq_query(server, datagram, datagram_size);
  if (response == NULL || !ssq_response_has_challenge(response))
    return response;
  challenge = ssq_response_get_challenge(response);
  ssq_response_free(response);
  return ssq_a2s_info_query(server, challenge);
}

static enum ssq_a2s_info_environment
ssq_a2s_info_read_environment(struct ssq_stream *const stream) {
  switch (ssq_stream_read_u8(stream)) {
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
ssq_a2s_info_read_server_type(struct ssq_stream *stream) {
  switch (ssq_stream_read_u8(stream)) {
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
ssq_a2s_info_deserialize(const struct ssq_response *const response,
                         struct ssq_error *const error) {
  struct ssq_stream stream;
  ssq_stream_initialize(&stream, response->data, response->size);
  if (ssq_response_is_truncated(response))
    ssq_stream_forward(&stream, sizeof(ssq_packet_header));
  const ssq_response_header header = ssq_stream_read_u8(&stream);
  if (header != SSQ_RESPONSE_HEADER_INFO) {
    ssq_error_initialize(error, SSQE_BAD,
                         "Unexpected A2S_INFO response header");
    return NULL;
  }
  struct ssq_a2s_info *const info = malloc(sizeof(*info));
  if (info == NULL) {
    ssq_error_initialize_errno(error);
    return NULL;
  }
  memset(info, 0, sizeof(*info));
  info->protocol = ssq_stream_read_u8(&stream);
  info->name = ssq_stream_read_string(&stream, &info->name_size);
  info->map = ssq_stream_read_string(&stream, &info->map_size);
  info->folder = ssq_stream_read_string(&stream, &info->folder_size);
  info->game = ssq_stream_read_string(&stream, &info->game_size);
  info->id = ssq_stream_read_u16(&stream);
  info->players = ssq_stream_read_u8(&stream);
  info->max_players = ssq_stream_read_u8(&stream);
  info->bots = ssq_stream_read_u8(&stream);
  info->server_type = ssq_a2s_info_read_server_type(&stream);
  info->environment = ssq_a2s_info_read_environment(&stream);
  info->password = ssq_stream_read_bool(&stream);
  info->secure = ssq_stream_read_bool(&stream);
  info->version = ssq_stream_read_string(&stream, &info->version_size);
  if (ssq_stream_available(&stream) == 0)
    return info;
  info->edf = ssq_stream_read_u8(&stream);
  if (ssq_a2s_info_has_port(info))
    info->port = ssq_stream_read_u16(&stream);
  if (ssq_a2s_info_has_steamid(info))
    info->steamid = ssq_stream_read_u64(&stream);
  if (ssq_a2s_info_has_source_tv(info)) {
    info->source_tv_port = ssq_stream_read_u16(&stream);
    info->source_tv_name =
        ssq_stream_read_string(&stream, &info->source_tv_name_size);
  }
  if (ssq_a2s_info_has_keywords(info))
    info->keywords = ssq_stream_read_string(&stream, &info->keywords_size);
  if (ssq_a2s_info_has_gameid(info))
    info->gameid = ssq_stream_read_u64(&stream);
  return info;
}

struct ssq_a2s_info *ssq_a2s_info(struct ssq_server *const server) {
  struct ssq_response *const response = ssq_a2s_info_query(server, 0xFFFFFFFF);
  if (response == NULL)
    return NULL;
  struct ssq_a2s_info *const info =
      ssq_a2s_info_deserialize(response, &server->last_error);
  ssq_response_free(response);
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

bool ssq_a2s_info_has_steamid(const struct ssq_a2s_info *const info) {
  return info->edf & SSQ_A2S_INFO_EDF_STEAMID;
}

bool ssq_a2s_info_has_source_tv(const struct ssq_a2s_info *const info) {
  return info->edf & SSQ_A2S_INFO_EDF_SOURCE_TV;
}
