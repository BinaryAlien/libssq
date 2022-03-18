#ifndef SSQ_PACKET_H
#define SSQ_PACKET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ssq/error.h"

#define A2S_PACKET_HEADER_SINGLE 0xFFFFFFFF
#define A2S_PACKET_HEADER_MULTI  0xFFFFFFFE

#define A2S_PACKET_ID_COMPRESSION_FLAG 0x80000000

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ssq_packet {
    int32_t  header;      /** The packet's header                            */
    int32_t  id;          /** Unique number assigned by server per answer    */
    uint8_t  total;       /** The total number of packets in the response    */
    uint8_t  number;      /** The number of the packet                       */
    uint16_t size;        /** Maximum size of packet before switching occurs */
    char    *payload;     /** The packet's payload                           */
    size_t   payload_len; /** Length of the packet's payload                 */
} SSQ_PACKET;

/**
 * Deserializes a packet from a datagram.
 * @param datagram     the datagram to use
 * @param datagram_len length of the datagram
 * @param err          where to report potential errors
 * @return a dynamically-allocated packet deserialized from `datagram`
 */
SSQ_PACKET *ssq_packet_init(const char datagram[], uint16_t datagram_len, SSQ_ERROR *err);

/**
 * Checks if every packet in an array of packets has the same ID.
 * @param packets      the array of packets
 * @param packet_count number of packets in the packets array
 * @return true if the packet IDs match
 */
bool ssq_packet_idcheck(const SSQ_PACKET *const *packets, uint8_t packet_count);

/**
 * Concatenates the payloads of an array of packets into a single heap-allocated buffer.
 * @param packets      the array of packets
 * @param packet_count number of packets in the packets array
 * @param out_len      length of the output buffer
 * @param err          where to report potential errors
 * @return a dynamically-allocated concatenation of all of the payloads in an array of packets
 */
char *ssq_packet_concatpayloads(const SSQ_PACKET *const *packets, uint8_t packet_count, size_t *out_len, SSQ_ERROR *err);

/**
 * Frees a packet.
 * @param packet the packet to free
 */
void ssq_packet_free(SSQ_PACKET *packet);

/**
 * Frees an array of packets.
 * @param packets      the array of packets to free
 * @param packet_count number of packets in the packets array
 */
void ssq_packet_freemany(SSQ_PACKET **packets, uint8_t packet_count);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_PACKET_H */
