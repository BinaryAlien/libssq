#include <stdlib.h>
#include <string.h>
#include "ssq/buf.h"
#include "ssq/helper.h"
#include "ssq/packet.h"

static void ssq_packet_init_payload(
    SSQ_PACKET *const packet,
    SSQ_BUF    *const datagram_buf,
    SSQ_ERROR  *const err
) {
    packet->payload = malloc(packet->payload_len);

    if (packet->payload != NULL)
        ssq_buf_get(packet->payload, datagram_buf, packet->payload_len);
    else
        ssq_error_set_sys(err);
}

static void ssq_packet_init_single(
    SSQ_PACKET *const packet,
    SSQ_BUF    *const datagram_buf,
    SSQ_ERROR  *const err
) {
    packet->total       = 1;
    packet->number      = 0;
    packet->payload_len = datagram_buf->size - datagram_buf->pos;

    ssq_packet_init_payload(packet, datagram_buf, err);
}

static void ssq_packet_init_multi(
    SSQ_PACKET *const packet,
    SSQ_BUF    *const datagram_buf,
    SSQ_ERROR  *const err
) {
    packet->id          = ssq_buf_get_int32(datagram_buf);
    packet->total       = ssq_buf_get_uint8(datagram_buf);
    packet->number      = ssq_buf_get_uint8(datagram_buf);
    packet->size        = ssq_buf_get_uint16(datagram_buf);
    packet->payload_len = MIN(packet->size, datagram_buf->size - datagram_buf->pos);

    if (packet->id & A2S_PACKET_ID_COMPRESSION_FLAG)
        ssq_error_set(err, SSQ_ERR_UNSUPPORTED, "Compressed responses are not supported");
    else
        ssq_packet_init_payload(packet, datagram_buf, err);
}

SSQ_PACKET *ssq_packet_init(
    const char       datagram[],
    const uint16_t   datagram_len,
    SSQ_ERROR *const err
) {
    SSQ_PACKET *packet = malloc(sizeof (*packet));

    if (packet != NULL) {
        memset(packet, 0, sizeof (*packet));

        SSQ_BUF datagram_buf = ssq_buf_init(datagram, datagram_len);

        packet->header = ssq_buf_get_int32(&datagram_buf);

        if (packet->header == A2S_PACKET_HEADER_SINGLE)
            ssq_packet_init_single(packet, &datagram_buf, err);
        else if (packet->header == A2S_PACKET_HEADER_MULTI)
            ssq_packet_init_multi(packet, &datagram_buf, err);
        else
            ssq_error_set(err, SSQ_ERR_BADRES, "Invalid packet header");

        if (err->code != SSQ_OK) {
            free(packet->payload);
            free(packet);
            packet = NULL;
        }
    } else {
        ssq_error_set_sys(err);
    }

    return packet;
}

bool ssq_packet_idcheck(const SSQ_PACKET *const *packets, uint8_t packet_count) {
    const uint32_t ref_id = packets[0]->id;

    for (uint8_t i = 1; i < packet_count; ++i)
        if (packets[i]->id != ref_id)
            return false;

    return true;
}

/* Computes the sum of the payload lengths of the packets in an array of packets */
static size_t ssq_packet_lensum(const SSQ_PACKET *const packets[], const uint8_t packet_count) {
    size_t len_sum = 0;

    for (uint8_t i = 0; i < packet_count; ++i)
         len_sum += packets[i]->payload_len;

    return len_sum;
}

char *ssq_packet_concatpayloads(
    const SSQ_PACKET *const packets[],
    const uint8_t           packet_count,
    size_t           *const out_len,
    SSQ_ERROR        *const err
) {
    *out_len = ssq_packet_lensum(packets, packet_count);

    char *const out = calloc(*out_len, sizeof (*out));

    if (out != NULL) {
        size_t copy_offset = 0;

        for (uint8_t i = 0; i < packet_count; ++i) {
            memcpy(out + copy_offset, packets[i]->payload, packets[i]->payload_len);
            copy_offset += packets[i]->payload_len;
        }
    } else {
        ssq_error_set_sys(err);
    }

    return out;
}

void ssq_packet_free(SSQ_PACKET *const packet) {
    free(packet->payload);
    free(packet);
}

void ssq_packet_freemany(SSQ_PACKET *packets[], const uint8_t packet_count) {
    for (uint8_t i = 0; i < packet_count; ++i)
        if (packets[i] != NULL)
            ssq_packet_free(packets[i]);

    free(packets);
}
