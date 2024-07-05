/* packet.h -- Representation of a Source server packet. */

#ifndef SSQ_PACKET_H
#define SSQ_PACKET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ssq/error.h"

#define SSQ_PACKET_SIZE 1400

#define SSQ_PACKET_HEADER_LEN    (sizeof (int32_t))
#define SSQ_PACKET_HEADER_SINGLE 0xFFFFFFFF
#define SSQ_PACKET_HEADER_MULTI  0xFFFFFFFE

#define SSQ_PACKET_FLAG_COMPRESSION 0x80000000

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ssq_packet {
    int32_t  header;      /* The packet's header.                            */
    int32_t  id;          /* Unique number assigned by server per answer.    */
    uint8_t  total;       /* The total number of packets in the response.    */
    uint8_t  number;      /* The number of the packet.                       */
    uint16_t size;        /* Maximum size of packet before switching occurs. */
    uint8_t *payload;     /* The packet's payload.                           */
    size_t   payload_len; /* Length of the packet's payload.                 */
} SSQ_PACKET;

SSQ_PACKET *ssq_packet_from_datagram(const uint8_t *datagram, uint16_t datagram_len, SSQ_ERROR *error);
void        ssq_packet_free(SSQ_PACKET *packet);

bool        ssq_packets_check_integrity(const SSQ_PACKET *const *packets, uint8_t packet_count);
uint8_t    *ssq_packets_to_response(const SSQ_PACKET *const *packets, uint8_t packet_count, size_t *response_len, SSQ_ERROR *error);
void        ssq_packets_free(SSQ_PACKET **packets, uint8_t packet_count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_PACKET_H */
