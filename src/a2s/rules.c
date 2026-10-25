#include "ssq/a2s/rules.h"

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

#define SSQ_A2S_RULES_HEADER_REQUEST 0x56
#define SSQ_A2S_RULES_HEADER_RESPONSE 0x45

static size_t ssq_a2s_rules_query_initialize(void *const datagram,
                                             const int32_t challenge) {
  struct ssq_cursor cursor;
  ssq_cursor_initialize(&cursor, datagram, SSQ_PACKET_SIZE);
  ssq_cursor_write_i32(&cursor, SSQ_PACKET_HEADER_SIMPLE);
  ssq_cursor_write_u8(&cursor, SSQ_A2S_RULES_HEADER_REQUEST);
  ssq_cursor_write(&cursor, &challenge, sizeof(challenge));
  return ssq_cursor_position(&cursor);
}

static struct ssq_buffer *ssq_a2s_rules_query(struct ssq_server *const server,
                                              int32_t challenge) {
  uint8_t datagram[SSQ_PACKET_SIZE];
  const size_t datagram_size =
      ssq_a2s_rules_query_initialize(datagram, challenge);
  struct ssq_buffer *const response =
      ssq_query(server, datagram, datagram_size);
  if (response == NULL || !ssq_response_has_challenge(response))
    return response;
  challenge = ssq_response_get_challenge(response);
  ssq_buffer_free(response);
  return ssq_a2s_rules_query(server, challenge);
}

struct ssq_a2s_rule *
ssq_a2s_rules_deserialize(const struct ssq_buffer *const response,
                          uint16_t *const count,
                          struct ssq_error *const error) {
  struct ssq_cursor cursor;
  ssq_cursor_initialize(&cursor, response->data, response->size);
  if (ssq_response_is_truncated(response))
    ssq_cursor_forward(&cursor, sizeof(int32_t));
  const uint8_t header = ssq_cursor_read_u8(&cursor);
  if (header != SSQ_A2S_RULES_HEADER_RESPONSE) {
    ssq_error_set(error, SSQE_BAD, "Bad A2S_RULES response header");
    return NULL;
  }
  *count = ssq_cursor_read_i16(&cursor);
  if (*count == 0)
    return NULL;
  struct ssq_a2s_rule *const rules = calloc(*count, sizeof(*rules));
  if (rules == NULL) {
    ssq_error_set_from_errno(error);
    return NULL;
  }
  for (uint16_t index = 0; index < *count; ++index) {
    rules[index].name =
        ssq_cursor_read_string(&cursor, &rules[index].name_size);
    rules[index].value =
        ssq_cursor_read_string(&cursor, &rules[index].value_size);
  }
  return rules;
}

struct ssq_a2s_rule *ssq_a2s_rules(struct ssq_server *const server,
                                   uint16_t *const count) {
  struct ssq_buffer *const response = ssq_a2s_rules_query(server, 0xFFFFFFFF);
  if (response == NULL)
    return NULL;
  struct ssq_a2s_rule *const rules =
      ssq_a2s_rules_deserialize(response, count, &server->last_error);
  ssq_buffer_free(response);
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
