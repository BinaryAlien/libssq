#include "ssq/packet.h"

#include <stdlib.h>
#include <string.h>

#include "ssq/buf.h"
#include "ssq/helper.h"

static void ssq_packet_init_payload(SSQ_PACKET *dest, SSQ_BUF *src, SSQ_ERROR *err) {
    dest->payload = malloc(dest->payload_len);
    if (dest->payload != NULL)
        ssq_buf_read(dest->payload, src, dest->payload_len);
    else
        ssq_error_set_from_errno(err);
}

static void ssq_packet_init_single(SSQ_PACKET *dest, SSQ_BUF *src, SSQ_ERROR *err) {
    dest->total       = 1;
    dest->number      = 0;
    dest->payload_len = src->size - src->cursor;
    ssq_packet_init_payload(dest, src, err);
}

static void ssq_packet_init_multi(SSQ_PACKET *dest, SSQ_BUF *src, SSQ_ERROR *err) {
    dest->id          = ssq_buf_read_int32(src);
    dest->total       = ssq_buf_read_uint8(src);
    dest->number      = ssq_buf_read_uint8(src);
    dest->size        = ssq_buf_read_uint16(src);
    dest->payload_len = ssq_helper_minz(dest->size, ssq_buf_available(src));
    if (dest->id & SSQ_PACKET_FLAG_COMPRESSION)
        ssq_error_set(err, SSQE_UNSUPPORTED, "Cannot process packet: decompression is not supported");
    else
        ssq_packet_init_payload(dest, src, err);
}

SSQ_PACKET *ssq_packet_from_datagram(const uint8_t datagram[], uint16_t datagram_len, SSQ_ERROR *err) {
    SSQ_PACKET *packet = malloc(sizeof (*packet));
    if (packet != NULL) {
        memset(packet, 0, sizeof (*packet));
        SSQ_BUF datagram_buf;
        ssq_buf_init(&datagram_buf, datagram, datagram_len);
        packet->header = ssq_buf_read_int32(&datagram_buf);
        if (packet->header == SSQ_PACKET_HEADER_SINGLE)
            ssq_packet_init_single(packet, &datagram_buf, err);
        else if (packet->header == SSQ_PACKET_HEADER_MULTI)
            ssq_packet_init_multi(packet, &datagram_buf, err);
        else
            ssq_error_set(err, SSQE_INVALID_RESPONSE, "Invalid packet header");
        if (err->code != SSQE_OK) {
            free(packet->payload);
            free(packet);
            packet = NULL;
        }
    } else {
        ssq_error_set_from_errno(err);
    }
    return packet;
}

void ssq_packet_free(SSQ_PACKET *packet) {
    free(packet->payload);
    free(packet);
}

bool ssq_packets_check_integrity(const SSQ_PACKET *const packets[], uint8_t packet_count) {
    for (uint8_t i = 1; i < packet_count; ++i) {
        if (packets[i]->id != packets[0]->id) {
            return false;
        }
    }
    return true;
}

static size_t ssq_packets_payload_len_sum(const SSQ_PACKET *const packets[], uint8_t packet_count) {
    size_t len = 0;
    for (uint8_t i = 0; i < packet_count; ++i)
        len += packets[i]->payload_len;
    return len;
}

uint8_t *ssq_packets_to_response(const SSQ_PACKET *const packets[], uint8_t packet_count, size_t *response_len, SSQ_ERROR *err) {
    *response_len = ssq_packets_payload_len_sum(packets, packet_count);
    uint8_t *response = malloc(*response_len);
    if (response != NULL) {
        size_t copy_offset = 0;
        for (uint8_t i = 0; i < packet_count; ++i) {
            memcpy(response + copy_offset, packets[i]->payload, packets[i]->payload_len);
            copy_offset += packets[i]->payload_len;
        }
    } else {
        ssq_error_set_from_errno(err);
    }
    return response;
}

void ssq_packets_free(SSQ_PACKET *packets[], uint8_t packet_count) {
    for (uint8_t i = 0; i < packet_count; ++i) {
        if (packets[i] != NULL) {
            ssq_packet_free(packets[i]);
        }
    }
    free(packets);
}
