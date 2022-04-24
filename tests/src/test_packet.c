#include <criterion/criterion.h>
#include "helper.h"
#include "ssq/packet.h"

Test(packet, deserialize_single) {
    size_t   datagram_len;
    uint8_t *datagram = read_datagram("dgram/info/css.bin", &datagram_len);

    uint8_t *payload     = datagram + 4;
    size_t   payload_len = datagram_len - 4;

    SSQ_ERROR err;
    ssq_error_clear(&err);
    SSQ_PACKET *const packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(packet, NULL);

    cr_expect_eq(packet->header, A2S_PACKET_HEADER_SINGLE);
    cr_expect_eq(packet->id, 0);
    cr_expect_eq(packet->total, 1);
    cr_expect_eq(packet->number, 0);
    cr_expect_eq(packet->size, 0);

    cr_assert_neq(packet->payload, NULL);
    cr_assert_eq(packet->payload_len, payload_len);
    cr_expect_arr_eq(packet->payload, payload, payload_len);

    free(datagram);
    ssq_packet_free(packet);
}

Test(packet, deserialize_multi) {
    size_t   datagram_len;
    uint8_t *datagram = read_datagram("dgram/rules/tf2_0.bin", &datagram_len);

    uint8_t *payload     = datagram + 12;
    size_t   payload_len = datagram_len - 12;

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(packet, NULL);

    cr_expect_eq(packet->header, A2S_PACKET_HEADER_MULTI);
    cr_expect_eq(packet->id, 0x033C);
    cr_expect_eq(packet->total, 5);
    cr_expect_eq(packet->number, 0);
    cr_expect_eq(packet->size, 0x04E0);

    cr_assert_neq(packet->payload, NULL);
    cr_assert_eq(packet->payload_len, payload_len);
    cr_expect_arr_eq(packet->payload, payload, payload_len);

    free(datagram);
    ssq_packet_free(packet);
}

Test(packet, verify_integrity) {
    const size_t packet_count = 5;
    SSQ_PACKET  *packets[packet_count];

    SSQ_ERROR err;
    ssq_error_clear(&err);

    for (size_t i = 0; i < packet_count; ++i) {
        const size_t filename_size = 32;
        char filename[filename_size];
        snprintf(filename, filename_size, "dgram/rules/tf2_%zu.bin", i);

        size_t   datagram_len;
        uint8_t *datagram = read_datagram(filename, &datagram_len);

        packets[i] = ssq_packet_from_datagram(datagram, datagram_len, &err);

        cr_assert_eq(err.code, SSQ_OK);
        cr_assert_neq(packets[i], NULL);

        free(datagram);
    }

    cr_expect(ssq_packets_verify_integrity((const SSQ_PACKET *const *)packets, packet_count));

    // Invalidate one of the packets' ID
    packets[0]->id = ~packets[0]->id;
    cr_expect(!ssq_packets_verify_integrity((const SSQ_PACKET *const *)packets, packet_count));

    for (size_t i = 0; i < packet_count; ++i)
        ssq_packet_free(packets[i]);
}

Test(packet, to_response) {
    const size_t packet_count = 5;
    SSQ_PACKET  *packets[packet_count];

    SSQ_ERROR err;
    ssq_error_clear(&err);

    for (size_t i = 0; i < packet_count; ++i) {
        const size_t filename_size = 32;
        char filename[filename_size];
        snprintf(filename, filename_size, "dgram/rules/tf2_%zu.bin", i);

        size_t   datagram_len;
        uint8_t *datagram = read_datagram(filename, &datagram_len);

        packets[i] = ssq_packet_from_datagram(datagram, datagram_len, &err);

        cr_assert_eq(err.code, SSQ_OK);
        cr_assert_neq(packets[i], NULL);

        free(datagram);
    }

    size_t   buf_len = 0;
    uint8_t *buf     = ssq_packets_to_response((const SSQ_PACKET *const *)packets, packet_count, &buf_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_eq(buf_len, 5723);
    cr_assert_neq(buf, NULL);

    cr_expect_arr_eq(buf, "\xFF\xFF\xFF\xFF\x45\xE0\x00", 7);

    cr_expect_arr_eq(buf + 7, "brimmunity_version", 19);
    cr_expect_arr_eq(buf + 26, "1.1.1p", 7);

    // ...

    cr_expect_arr_eq(buf + 5704, "tv_relaypassword", 17);
    cr_expect_arr_eq(buf + 5721, "0", 2);

    for (size_t i = 0; i < packet_count; ++i)
        ssq_packet_free(packets[i]);
}
