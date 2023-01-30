#include "ssq/packet.h"

#include <stdlib.h>
#include <string.h>

#include "ssq/helper.h"
#include "ssq/stream.h"

static void ssq_packet_init_payload(SSQ_PACKET *packet, SSQ_STREAM *stream, SSQ_ERROR *err) {
    packet->payload = malloc(packet->payload_len);
    if (packet->payload != NULL)
        ssq_stream_read(stream, packet->payload, packet->payload_len);
    else
        ssq_error_set_from_errno(err);
}

static void ssq_packet_init_single(SSQ_PACKET *packet, SSQ_STREAM *stream, SSQ_ERROR *err) {
    packet->total       = 1;
    packet->number      = 0;
    packet->payload_len = ssq_stream_remaining(stream);
    ssq_packet_init_payload(packet, stream, err);
}

static void ssq_packet_init_multi(SSQ_PACKET *packet, SSQ_STREAM *stream, SSQ_ERROR *err) {
    packet->id          = ssq_stream_read_int32_t(stream);
    packet->total       = ssq_stream_read_uint8_t(stream);
    packet->number      = ssq_stream_read_uint8_t(stream);
    packet->size        = ssq_stream_read_uint16_t(stream);
    packet->payload_len = ssq_helper_minz(packet->size, ssq_stream_remaining(stream));
    if (packet->id & SSQ_PACKET_FLAG_COMPRESSION)
        ssq_error_set(err, SSQE_UNSUPPORTED, "Cannot process packet: decompression is not supported");
    else
        ssq_packet_init_payload(packet, stream, err);
}

SSQ_PACKET *ssq_packet_from_datagram(const uint8_t datagram[], uint16_t datagram_len, SSQ_ERROR *err) {
    SSQ_PACKET *packet = malloc(sizeof (*packet));
    if (packet != NULL) {
        memset(packet, 0, sizeof (*packet));
        SSQ_STREAM datagram_stream;
        ssq_stream_wrap(&datagram_stream, datagram, datagram_len);
        packet->header = ssq_stream_read_int32_t(&datagram_stream);
        if (packet->header == SSQ_PACKET_HEADER_SINGLE)
            ssq_packet_init_single(packet, &datagram_stream, err);
        else if (packet->header == SSQ_PACKET_HEADER_MULTI)
            ssq_packet_init_multi(packet, &datagram_stream, err);
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

uint8_t *ssq_packets_to_response(const SSQ_PACKET *const packets[], uint8_t packet_count,
                                 size_t *response_len, SSQ_ERROR *err) {
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
