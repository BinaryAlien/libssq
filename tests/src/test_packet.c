#include <criterion/criterion.h>
#include "ssq/packet.h"

static size_t _memcpy(
    void       *const dst,
    const void *const src,
    const size_t      n
) {
    memcpy(dst, src, n);
    return n;
}

static SSQ_PACKET **helper_init_empty(const uint8_t packet_count) {
    SSQ_PACKET **const packets = calloc(packet_count, sizeof (SSQ_PACKET *));

    for (uint8_t i = 0; i < packet_count; ++i) {
        packets[i] = malloc(sizeof (SSQ_PACKET));
        memset(packets[i], 0, sizeof (SSQ_PACKET));
    }

    return packets;
}

static size_t helper_serialize_single(const SSQ_PACKET *const packet, uint8_t buf[]) {
    size_t len = 0;

    len += _memcpy(buf + len, &(packet->header), sizeof (packet->header));
    len += _memcpy(buf + len, packet->payload,   packet->payload_len);

    return len;
}

static size_t helper_serialize_multi(const SSQ_PACKET *const packet, uint8_t buf[]) {
    size_t len = 0;

    len += _memcpy(buf + len, &(packet->header), sizeof (packet->header));
    len += _memcpy(buf + len, &(packet->id),     sizeof (packet->id));
    len += _memcpy(buf + len, &(packet->total),  sizeof (packet->total));
    len += _memcpy(buf + len, &(packet->number), sizeof (packet->number));
    len += _memcpy(buf + len, &(packet->size),   sizeof (packet->size));
    len += _memcpy(buf + len, packet->payload,   packet->payload_len);

    return len;
}

static void helper_expect_packets_eq(const SSQ_PACKET *const actual, const SSQ_PACKET *const expected) {
    cr_expect_eq(actual->header,      expected->header);
    cr_expect_eq(actual->id,          expected->id);
    cr_expect_eq(actual->total,       expected->total);
    cr_expect_eq(actual->number,      expected->number);
    cr_expect_eq(actual->size,        expected->size);

    cr_assert_neq(actual->payload,    NULL);

    cr_assert_eq(actual->payload_len, expected->payload_len);
    cr_expect_arr_eq(actual->payload, expected->payload, expected->payload_len);
}

Test(packet, deserialize_single, .description = "Single-response packet deserialization") {
    uint8_t ref_payload[] = { 0x02, 0xCA, 0xFE, 0x10 };

    const SSQ_PACKET ref = {
        .header      = A2S_PACKET_HEADER_SINGLE,
        .id          = 0,
        .total       = 1,
        .number      = 0,
        .size        = 0,
        .payload     = ref_payload,
        .payload_len = sizeof (ref_payload)
    };

    uint8_t      datagram[64];
    const size_t datagram_len = helper_serialize_single(&ref, datagram);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *const packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(packet, NULL);

    helper_expect_packets_eq(packet, &ref);

    ssq_packet_free(packet);
}

Test(packet, deserialize_multi, .description = "Multi-response packet deserialization") {
    uint8_t ref_payload[] = { 0x02, 0xCA, 0xFE, 0x10 };

    const SSQ_PACKET ref = {
        .header      = A2S_PACKET_HEADER_MULTI,
        .id          = 0x12345678,
        .total       = 3,
        .number      = 2,
        .size        = 1260,
        .payload     = ref_payload,
        .payload_len = sizeof (ref_payload)
    };

    uint8_t      datagram[128];
    const size_t datagram_len = helper_serialize_multi(&ref, datagram);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *const packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(packet, NULL);

    helper_expect_packets_eq(packet, &ref);

    ssq_packet_free(packet);
}

Test(packet, deserialized_compressed, .description = "Multi-response compressed packet deserialization") {
    uint8_t ref_payload[] = { 0x02, 0xCA, 0xFE, 0x10 };

    const SSQ_PACKET ref = {
        .header      = A2S_PACKET_HEADER_MULTI,
        .id          = 0x12345678 | A2S_PACKET_FLAG_COMPRESSION,
        .total       = 3,
        .number      = 0,
        .size        = 1260,
        .payload     = ref_payload,
        .payload_len = 9
    };

    uint8_t      datagram[128];
    const size_t datagram_len = helper_serialize_multi(&ref, datagram);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *const packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_expect_eq(err.code, SSQ_ERR_UNSUPPORTED);
    cr_expect_eq(packet, NULL);
}

Test(packet, verify_integrity, .description = "Packets integrity check") {
    const uint8_t       packet_count = 4;
    SSQ_PACKET  **const packets      = helper_init_empty(packet_count);

    const int32_t ref_id = 0x12345678;

    for (uint8_t i = 0; i < packet_count; ++i)
        packets[i]->id = ref_id;

    const SSQ_PACKET *const *const packets_readonly = (const SSQ_PACKET *const *)packets;

    cr_expect(ssq_packets_verify_integrity(packets_readonly, packet_count));

    // invalidate a packet's ID
    packets[0]->id = ~ref_id;

    cr_expect(!ssq_packets_verify_integrity(packets_readonly, packet_count));

    ssq_packets_free(packets, packet_count);
}

Test(packet, to_response, .description = "Response buffer generation") {
    const size_t  payload_fragment_len = 3;
    const uint8_t packet_count         = 3;
    const size_t  payload_len          = payload_fragment_len * packet_count;

    const uint8_t payload[] = {
        0x12, 0x34, 0x56, // Fragment #0
        0x23, 0x45, 0x67, // Fragment #1
        0x34, 0x56, 0x78  // Fragment #2
    };

    SSQ_PACKET **const packets = helper_init_empty(packet_count);

    // Load the payload fragments in their respective packet
    for (uint8_t i = 0; i < packet_count; ++i) {
        const uint8_t *const payload_fragment = payload + i * payload_fragment_len;

        packets[i]->payload_len = payload_fragment_len;
        packets[i]->payload     = malloc(packets[i]->payload_len);

        memcpy(packets[i]->payload, payload_fragment, payload_fragment_len);
    }

    SSQ_ERROR err;
    ssq_error_clear(&err);

    size_t         out_len = 0;
    uint8_t *const out     = ssq_packets_to_response((const SSQ_PACKET *const *)packets, packet_count, &out_len, &err);

    cr_expect_eq(err.code, SSQ_OK);
    cr_assert_neq(out, NULL);

    cr_assert_eq(out_len, payload_len);
    cr_expect_arr_eq(out, payload, payload_len);

    free(out);
    ssq_packets_free(packets, packet_count);
}
