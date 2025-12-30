#include "ssq/a2s/rules.h"

#include <stdlib.h>
#include <string.h>

#include "packet.h"
#include "query.h"
#include "response.h"
#include "server.h"
#include "stream.h"

#define SSQ_REQUEST_HEADER_RULES 0x56
#define SSQ_RESPONSE_HEADER_RULES 0x45

static struct ssq_response *
ssq_a2s_rules_query(struct ssq_server *const server,
                    ssq_response_challenge challenge) {
  uint8_t datagram[SSQ_PACKET_SIZE];
  struct ssq_stream datagram_stream;
  ssq_stream_initialize(&datagram_stream, datagram, sizeof(datagram));
  ssq_stream_write_i32(&datagram_stream, SSQ_PACKET_HEADER_SIMPLE);
  ssq_stream_write_u8(&datagram_stream, SSQ_REQUEST_HEADER_RULES);
  ssq_stream_write(&datagram_stream, &challenge, sizeof(challenge));
  const size_t datagram_size = ssq_stream_position(&datagram_stream);
  struct ssq_response *const response =
      ssq_query(server, datagram, datagram_size);
  if (response == NULL || !ssq_response_has_challenge(response))
    return response;
  challenge = ssq_response_get_challenge(response);
  ssq_response_free(response);
  return ssq_a2s_rules_query(server, challenge);
}

struct ssq_a2s_rule *
ssq_a2s_rules_deserialize(const struct ssq_response *const response,
                          uint16_t *const count,
                          struct ssq_error *const error) {
  struct ssq_stream stream;
  ssq_stream_initialize(&stream, response->data, response->size);
  if (ssq_response_is_truncated(response))
    ssq_stream_forward(&stream, sizeof(ssq_packet_header));
  const ssq_response_header header = ssq_stream_read_u8(&stream);
  if (header != SSQ_RESPONSE_HEADER_RULES) {
    ssq_error_set(error, SSQE_BAD, "Unexpected A2S_RULES response header");
    return NULL;
  }
  *count = ssq_stream_read_i16(&stream);
  if (*count == 0)
    return NULL;
  struct ssq_a2s_rule *const rules = calloc(*count, sizeof(*rules));
  if (rules == NULL) {
    ssq_error_set_from_errno(error);
    return NULL;
  }
  for (uint16_t index = 0; index < *count; ++index) {
    rules[index].name =
        ssq_stream_read_string(&stream, &rules[index].name_size);
    rules[index].value =
        ssq_stream_read_string(&stream, &rules[index].value_size);
  }
  return rules;
}

struct ssq_a2s_rule *ssq_a2s_rules(struct ssq_server *const server,
                                   uint16_t *const count) {
  struct ssq_response *const response = ssq_a2s_rules_query(server, 0xFFFFFFFF);
  if (response == NULL)
    return NULL;
  struct ssq_a2s_rule *const rules =
      ssq_a2s_rules_deserialize(response, count, &server->last_error);
  ssq_response_free(response);
  return rules;
}

void ssq_a2s_rules_free(struct ssq_a2s_rule *const rules,
                        const uint16_t count) {
  if (rules == NULL)
    return;
  for (uint16_t index = 0; index < count; ++index) {
    free(rules[index].name);
    free(rules[index].value);
  }
  free(rules);
}
