#include "ssq/a2s/player.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "packet.h"
#include "query.h"
#include "response.h"
#include "server.h"
#include "stream.h"

#define SSQ_REQUEST_HEADER_PLAYER 0x55
#define SSQ_RESPONSE_HEADER_PLAYER 0x44

static struct ssq_response *
ssq_a2s_player_query(struct ssq_server *const server,
                     ssq_response_challenge challenge) {
  uint8_t datagram[SSQ_PACKET_SIZE];
  struct ssq_stream datagram_stream;
  ssq_stream_initialize(&datagram_stream, datagram, sizeof(datagram));
  ssq_stream_write_i32(&datagram_stream, SSQ_PACKET_HEADER_SIMPLE);
  ssq_stream_write_u8(&datagram_stream, SSQ_REQUEST_HEADER_PLAYER);
  ssq_stream_write(&datagram_stream, &challenge, sizeof(challenge));
  const size_t datagram_size = ssq_stream_position(&datagram_stream);
  struct ssq_response *const response =
      ssq_query(server, datagram, datagram_size);
  if (response == NULL || !ssq_response_has_challenge(response))
    return response;
  challenge = ssq_response_get_challenge(response);
  ssq_response_free(response);
  return ssq_a2s_player_query(server, challenge);
}

struct ssq_a2s_player *
ssq_a2s_player_deserialize(const struct ssq_response *const response,
                           uint8_t *const count,
                           struct ssq_error *const error) {
  struct ssq_stream stream;
  ssq_stream_initialize(&stream, response->data, response->size);
  if (ssq_response_is_truncated(response))
    ssq_stream_forward(&stream, sizeof(ssq_packet_header));
  const ssq_response_header header = ssq_stream_read_u8(&stream);
  if (header != SSQ_RESPONSE_HEADER_PLAYER) {
    ssq_error_initialize(error, SSQE_BAD,
                         "Unexpected A2S_PLAYER response header");
    return NULL;
  }
  *count = ssq_stream_read_u8(&stream);
  if (*count == 0)
    return NULL;
  struct ssq_a2s_player *const players = calloc(*count, sizeof(*players));
  if (players == NULL) {
    ssq_error_initialize_errno(error);
    return NULL;
  }
  for (uint8_t index = 0; index < *count; ++index) {
    players[index].index = ssq_stream_read_u8(&stream);
    players[index].name =
        ssq_stream_read_string(&stream, &players[index].name_size);
    players[index].score = ssq_stream_read_i32(&stream);
    players[index].duration = ssq_stream_read_f32(&stream);
  }
  return players;
}

struct ssq_a2s_player *ssq_a2s_player(struct ssq_server *const server,
                                      uint8_t *const count) {
  struct ssq_response *const response =
      ssq_a2s_player_query(server, 0xFFFFFFFF);
  if (response == NULL)
    return NULL;
  struct ssq_a2s_player *const players =
      ssq_a2s_player_deserialize(response, count, &server->last_error);
  ssq_response_free(response);
  return players;
}

void ssq_a2s_player_free(struct ssq_a2s_player *const players,
                         const uint8_t count) {
  if (players == NULL)
    return;
  for (uint8_t index = 0; index < count; ++index)
    free(players[index].name);
  free(players);
}
