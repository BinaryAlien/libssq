#ifndef SSQ_PACKET_H
#define SSQ_PACKET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ssq/error.h"

#define A2S_PACKET_HEADER_SINGLE 0xFFFFFFFF
#define A2S_PACKET_HEADER_MULTI  0xFFFFFFFE

#define A2S_PACKET_FLAG_COMPRESSION 0x80000000

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ssq_packet {
    int32_t  header;      /** The packet's header                            */
    int32_t  id;          /** Unique number assigned by server per answer    */
    uint8_t  total;       /** The total number of packets in the response    */
    uint8_t  number;      /** The number of the packet                       */
    uint16_t size;        /** Maximum size of packet before switching occurs */
    uint8_t *payload;     /** The packet's payload                           */
    size_t   payload_len; /** Length of the packet's payload                 */
} SSQ_PACKET;

/**
 * Deserializes a packet from a datagram.
 *
 * @param datagram     source datagram
 * @param datagram_len length of the source datagram
 * @param err          where to report potential errors
 *
 * @return dynamically-allocated packet deserialized from the source datagram
 */
SSQ_PACKET *ssq_packet_from_datagram(const uint8_t *datagram, uint16_t datagram_len, SSQ_ERROR *err);

/**
 * Verifies that each packet matches the ID of the first one.
 *
 * @param packets      array of packets
 * @param packet_count number of packets in the packet array
 *
 * @return true if the packet IDs match
 */
bool ssq_packets_verify_integrity(const SSQ_PACKET *const *packets, uint8_t packet_count);

/**
 * Generates a buffer from the payloads of an array of packets.
 *
 * @param packets      array of packets
 * @param packet_count number of packets in the packet array
 * @param buf_len      where to store the length of the buffer
 * @param err          where to report potential errors
 *
 * @return dynamically-allocated buffer made of the packets payloads' concatenation
 */
uint8_t *ssq_packets_to_response(const SSQ_PACKET *const *packets, uint8_t packet_count, size_t *buf_len, SSQ_ERROR *err);

/**
 * Frees a packet.
 * @param packet packet to free
 */
void ssq_packet_free(SSQ_PACKET *packet);

/**
 * Frees an array of packets.
 * @param packets      array of packets to free
 * @param packet_count number of packets in the packet array
 */
void ssq_packets_free(SSQ_PACKET **packets, uint8_t packet_count);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_PACKET_H */
