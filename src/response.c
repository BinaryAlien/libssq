#include "response.h"

#include <stddef.h>
#include <stdint.h>

#include "buffer.h"
#include "cursor.h"
#include "packet.h"

bool ssq_response_has_challenge(const struct ssq_buffer *const response) {
  struct ssq_cursor cursor;
  ssq_cursor_initialize(&cursor, response->data, response->size);
  const uint8_t header = ssq_cursor_read_u8(&cursor);
  return header == SSQ_RESPONSE_HEADER_CHALLENGE;
}

int32_t ssq_response_get_challenge(const struct ssq_buffer *const response) {
  struct ssq_cursor cursor;
  ssq_cursor_initialize(&cursor, response->data, response->size);
  ssq_cursor_forward(&cursor, sizeof(uint8_t));
  return ssq_cursor_read_i32(&cursor);
}

bool ssq_response_is_truncated(const struct ssq_buffer *const response) {
  struct ssq_cursor cursor;
  ssq_cursor_initialize(&cursor, response->data, response->size);
  const int32_t potential_header = ssq_cursor_read_i32(&cursor);
  return potential_header == SSQ_PACKET_HEADER_SIMPLE;
}
