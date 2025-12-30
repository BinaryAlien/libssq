#ifndef PACKET_H
#define PACKET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ssq/error.h"

#define SSQ_PACKET_HEADER_SIMPLE 0xFFFFFFFF
#define SSQ_PACKET_HEADER_SPLIT 0xFFFFFFFE

#define SSQ_PACKET_SIZE 1400

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int32_t ssq_packet_header;

struct ssq_packet {
  /* The packet's header. */
  ssq_packet_header header;
  /* Unique number assigned by server per answer. */
  int32_t id;
  /* The total number of packets in the response. */
  uint8_t total;
  /* The number of the packet. */
  uint8_t number;
  /* The packet's payload. */
  uint8_t *payload;
  /* Size in bytes of the packet's payload. */
  size_t size;
};

struct ssq_packet *ssq_packet_from_buffer(const uint8_t *buffer, uint16_t size,
                                          struct ssq_error *error);

void ssq_packet_free(struct ssq_packet *packet);

bool ssq_packets_check(const struct ssq_packet *const *packets);

size_t ssq_packets_size(const struct ssq_packet *const *packets);

void ssq_packets_merge(const struct ssq_packet *const *packets,
                       uint8_t *buffer);

void ssq_packets_free(struct ssq_packet **packets, const uint8_t count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !PACKET_H */
