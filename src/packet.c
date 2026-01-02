#include "packet.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "stream.h"

#define SSQ_PACKET_FLAG_COMPRESSION 0x80000000

static struct ssq_packet *ssq_packet_alloc(void) {
  struct ssq_packet *const packet = malloc(sizeof(*packet));
  if (packet == NULL)
    return NULL;
  memset(packet, 0, sizeof(*packet));
  return packet;
}

static bool ssq_packet_read_payload(struct ssq_packet *const packet,
                                    struct ssq_stream *const stream,
                                    struct ssq_error *const error) {
  packet->payload = malloc(packet->size);
  if (packet->payload == NULL) {
    ssq_error_initialize_errno(error);
    return false;
  }
  ssq_stream_read(stream, packet->payload, packet->size);
  return true;
}

static bool ssq_packet_read_simple(struct ssq_packet *const packet,
                                   struct ssq_stream *const stream,
                                   struct ssq_error *const error) {
  packet->total = 1;
  packet->number = 0;
  packet->size = ssq_stream_available(stream);
  return ssq_packet_read_payload(packet, stream, error);
}

static bool ssq_packet_read_split(struct ssq_packet *const packet,
                                  struct ssq_stream *const stream,
                                  struct ssq_error *const error) {
  packet->id = ssq_stream_read_i32(stream);
  packet->total = ssq_stream_read_u8(stream);
  packet->number = ssq_stream_read_u8(stream);
  ssq_stream_forward(stream, sizeof(uint16_t));
  packet->size = ssq_stream_available(stream);
  if (packet->id & SSQ_PACKET_FLAG_COMPRESSION) {
    ssq_error_initialize(error, SSQE_UNSUPPORTED,
                         "Unsupported packet compression");
    return false;
  }
  return ssq_packet_read_payload(packet, stream, error);
}

static bool ssq_packet_read(struct ssq_packet *const packet,
                            struct ssq_stream *const stream,
                            struct ssq_error *const error) {
  switch (packet->header = ssq_stream_read_i32(stream)) {
  case SSQ_PACKET_HEADER_SIMPLE:
    return ssq_packet_read_simple(packet, stream, error);
  case SSQ_PACKET_HEADER_SPLIT:
    return ssq_packet_read_split(packet, stream, error);
  default:
    ssq_error_initialize(error, SSQE_BAD, "Unexpected packet header");
    return false;
  }
}

struct ssq_packet *ssq_packet_from_stream(struct ssq_stream *const stream,
                                          struct ssq_error *const error) {
  struct ssq_packet *const packet = ssq_packet_alloc();
  if (packet == NULL) {
    ssq_error_initialize_errno(error);
    return NULL;
  }
  ssq_packet_read(packet, stream, error);
  return packet;
}

struct ssq_packet *ssq_packet_from_buffer(const void *const buffer,
                                          const size_t size,
                                          struct ssq_error *const error) {
  struct ssq_stream stream;
  ssq_stream_initialize(&stream, buffer, size);
  return ssq_packet_from_stream(&stream, error);
}

void ssq_packet_free(struct ssq_packet *packet) {
  if (packet == NULL)
    return;
  free(packet->payload);
  free(packet);
}

bool ssq_packets_check(const struct ssq_packet *const *const packets) {
  for (uint8_t number = 1; number < packets[0]->total; ++number)
    if (packets[number]->id != packets[0]->id)
      return false;
  return true;
}

size_t ssq_packets_size(const struct ssq_packet *const *const packets) {
  size_t size = 0;
  for (uint8_t number = 0; number < packets[0]->total; ++number)
    size += packets[number]->size;
  return size;
}

void ssq_packets_merge(const struct ssq_packet *const *const packets,
                       void *const buffer) {
  uint8_t *cursor = buffer;
  for (uint8_t number = 0; number < packets[0]->total; ++number) {
    const size_t size = packets[number]->size;
    memcpy(cursor, packets[number]->payload, size);
    cursor += size;
  }
}

void ssq_packets_free(struct ssq_packet **const packets, const size_t count) {
  for (size_t index = 0; index < count; ++index)
    ssq_packet_free(packets[index]);
  free(packets);
}
