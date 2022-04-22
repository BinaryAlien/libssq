#include <stdlib.h>
#include <string.h>
#include "ssq/buf.h"
#include "ssq/helper.h"
#include "ssq/packet.h"

static void ssq_packet_init_payload(
    SSQ_PACKET *const dst,
    SSQ_BUF    *const src,
    SSQ_ERROR  *const err
) {
    dst->payload = malloc(dst->payload_len);

    if (dst->payload != NULL)
        ssq_buf_get(dst->payload, src, dst->payload_len);
    else
        ssq_error_set_from_errno(err);
}

static void ssq_packet_init_single(
    SSQ_PACKET *const dst,
    SSQ_BUF    *const src,
    SSQ_ERROR  *const err
) {
    dst->total       = 1;
    dst->number      = 0;
    dst->payload_len = src->payload_len - src->cursor;

    ssq_packet_init_payload(dst, src, err);
}

static void ssq_packet_init_multi(
    SSQ_PACKET *const dst,
    SSQ_BUF    *const src,
    SSQ_ERROR  *const err
) {
    dst->id          = ssq_buf_get_int32(src);
    dst->total       = ssq_buf_get_uint8(src);
    dst->number      = ssq_buf_get_uint8(src);
    dst->size        = ssq_buf_get_uint16(src);
    dst->payload_len = ssq_helper_minz(dst->size, ssq_buf_available(src));

    if (dst->id & A2S_PACKET_FLAG_COMPRESSION)
        ssq_error_set(err, SSQ_ERR_UNSUPPORTED, "Compressed responses are not supported");
    else
        ssq_packet_init_payload(dst, src, err);
}

SSQ_PACKET *ssq_packet_from_datagram(
    const uint8_t    datagram[],
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
        ssq_error_set_from_errno(err);
    }

    return packet;
}

bool ssq_packets_verify_integrity(const SSQ_PACKET *const packets[], const uint8_t packet_count) {
    for (uint8_t i = 1; i < packet_count; ++i) {
        if (packets[i]->id != packets[0]->id) {
            return false;
        }
    }

    return true;
}

/**
 * Computes the sum of the lengths of the payloads in an array of packets.
 *
 * @param packets      array of packets
 * @param packet_count number of packets in the packet array
 *
 * @return sum of the lengths of the payloads in the array of packets
 */
static size_t ssq_packets_payload_len_sum(const SSQ_PACKET *const packets[], const uint8_t packet_count) {
    size_t len = 0;

    for (uint8_t i = 0; i < packet_count; ++i)
        len += packets[i]->payload_len;

    return len;
}

uint8_t *ssq_packets_to_response(
    const SSQ_PACKET *const packets[],
    const uint8_t           packet_count,
    size_t           *const buf_len,
    SSQ_ERROR        *const err
) {
    *buf_len = ssq_packets_payload_len_sum(packets, packet_count);

    uint8_t *const out = malloc(*buf_len);

    if (out != NULL) {
        size_t copy_offset = 0;

        for (uint8_t i = 0; i < packet_count; ++i) {
            memcpy(out + copy_offset, packets[i]->payload, packets[i]->payload_len);
            copy_offset += packets[i]->payload_len;
        }
    } else {
        ssq_error_set_from_errno(err);
    }

    return out;
}

void ssq_packet_free(SSQ_PACKET *const packet) {
    free(packet->payload);
    free(packet);
}

void ssq_packets_free(SSQ_PACKET *packets[], const uint8_t packet_count) {
    for (uint8_t i = 0; i < packet_count; ++i) {
        if (packets[i] != NULL) {
            ssq_packet_free(packets[i]);
        }
    }

    free(packets);
}
