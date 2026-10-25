#include "packet.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "cursor.h"
#include "error.h"

#define SSQ_PACKET_FLAG_COMPRESSION 0x80000000

static struct ssq_packet *ssq_packet_alloc(void) {
  struct ssq_packet *const packet = malloc(sizeof(*packet));
  if (packet == NULL)
    return NULL;
  memset(packet, 0, sizeof(*packet));
  return packet;
}

static bool ssq_packet_read_payload(struct ssq_packet *const packet,
                                    struct ssq_cursor *const cursor,
                                    struct ssq_error *const error) {
  packet->payload = malloc(packet->size);
  if (packet->payload == NULL) {
    ssq_error_set_from_errno(error);
    return false;
  }
  ssq_cursor_read(cursor, packet->payload, packet->size);
  return true;
}

static bool ssq_packet_read_simple(struct ssq_packet *const packet,
                                   struct ssq_cursor *const cursor,
                                   struct ssq_error *const error) {
  packet->total = 1;
  packet->number = 0;
  packet->size = ssq_cursor_available(cursor);
  return ssq_packet_read_payload(packet, cursor, error);
}

static bool ssq_packet_read_split(struct ssq_packet *const packet,
                                  struct ssq_cursor *const cursor,
                                  struct ssq_error *const error) {
  packet->id = ssq_cursor_read_i32(cursor);
  packet->total = ssq_cursor_read_u8(cursor);
  packet->number = ssq_cursor_read_u8(cursor);
  ssq_cursor_forward(cursor, sizeof(uint16_t));
  packet->size = ssq_cursor_available(cursor);
  if (packet->id & SSQ_PACKET_FLAG_COMPRESSION) {
    ssq_error_set(error, SSQE_UNSUPPORTED, "Cannot process compressed packets");
    return false;
  }
  return ssq_packet_read_payload(packet, cursor, error);
}

static bool ssq_packet_read(struct ssq_packet *const packet,
                            struct ssq_cursor *const cursor,
                            struct ssq_error *const error) {
  switch (packet->header = ssq_cursor_read_i32(cursor)) {
  case SSQ_PACKET_HEADER_SIMPLE:
    return ssq_packet_read_simple(packet, cursor, error);
  case SSQ_PACKET_HEADER_SPLIT:
    return ssq_packet_read_split(packet, cursor, error);
  default:
    ssq_error_set(error, SSQE_BAD, "Bad packet header");
    return false;
  }
}

struct ssq_packet *ssq_packet_from_cursor(struct ssq_cursor *const cursor,
                                          struct ssq_error *const error) {
  struct ssq_packet *const packet = ssq_packet_alloc();
  if (packet == NULL) {
    ssq_error_set_from_errno(error);
    return NULL;
  }
  ssq_packet_read(packet, cursor, error);
  return packet;
}

struct ssq_packet *ssq_packet_from_buffer(const void *const buffer,
                                          const size_t buffer_size,
                                          struct ssq_error *const error) {
  struct ssq_cursor cursor;
  ssq_cursor_initialize(&cursor, buffer, buffer_size);
  return ssq_packet_from_cursor(&cursor, error);
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

static size_t ssq_packets_size(const struct ssq_packet *const *const packets) {
  size_t size = 0;
  for (uint8_t number = 0; number < packets[0]->total; ++number)
    size += packets[number]->size;
  return size;
}

struct ssq_buffer *ssq_packets_merge(const struct ssq_packet *const *packets) {
  const size_t response_size = ssq_packets_size(packets);
  struct ssq_buffer *response = ssq_buffer_alloc(response_size);
  if (response == NULL)
    return NULL;
  uint8_t *cursor = response->data;
  for (uint8_t number = 0; number < packets[0]->total; ++number) {
    memcpy(cursor, packets[number]->payload, packets[number]->size);
    cursor += packets[number]->size;
  }
  return response;
}

void ssq_packets_free(struct ssq_packet **const packets, const size_t count) {
  for (size_t index = 0; index < count; ++index)
    ssq_packet_free(packets[index]);
  free(packets);
}
