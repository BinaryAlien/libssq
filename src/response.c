#include "response.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "packet.h"
#include "stream.h"

static struct ssq_response *ssq_response_alloc(const size_t response_size) {
  struct ssq_response *const response = malloc(sizeof(*response));
  if (response == NULL)
    return NULL;
  response->size = response_size;
  response->data = malloc(response_size);
  if (response->data == NULL) {
    free(response);
    return NULL;
  }
  return response;
}

struct ssq_response *
ssq_response_from_packets(const struct ssq_packet *const *const packets) {
  const size_t response_size = ssq_packets_size(packets);
  struct ssq_response *const response = ssq_response_alloc(response_size);
  if (response == NULL)
    return NULL;
  ssq_packets_merge(packets, response->data);
  return response;
}

void ssq_response_free(struct ssq_response *const response) {
  if (response == NULL)
    return;
  free(response->data);
  free(response);
}

bool ssq_response_has_challenge(const struct ssq_response *const response) {
  struct ssq_stream stream;
  ssq_stream_initialize(&stream, response->data, response->size);
  const ssq_response_header header = ssq_stream_read_u8(&stream);
  return header == SSQ_RESPONSE_HEADER_CHALLENGE;
}

ssq_response_challenge
ssq_response_get_challenge(const struct ssq_response *const response) {
  struct ssq_stream stream;
  ssq_stream_initialize(&stream, response->data, response->size);
  ssq_stream_forward(&stream, sizeof(ssq_response_header));
  return ssq_stream_read_i32(&stream);
}

bool ssq_response_is_truncated(const struct ssq_response *const response) {
  struct ssq_stream stream;
  ssq_stream_initialize(&stream, response->data, response->size);
  const ssq_packet_header potential_header = ssq_stream_read_i32(&stream);
  return potential_header == SSQ_PACKET_HEADER_SIMPLE;
}
