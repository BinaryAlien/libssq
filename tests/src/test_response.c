#include <criterion/criterion.h>
#include "helper.h"
#include "ssq/packet.h"
#include "ssq/response.h"

Test(response, has_challenge) {
    size_t   datagram_len;
    uint8_t *datagram = read_datagram("dgram/chall/example_0.bin", &datagram_len);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(packet, NULL);

    size_t   response_len;
    uint8_t *response = ssq_packets_to_response((const SSQ_PACKET *const *)(&packet), 1, &response_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(response, NULL);

    cr_expect(ssq_response_has_challenge(response, response_len));

    free(datagram);

    ssq_packet_free(packet);

    free(response);
}

Test(response, has_no_challenge) {
    size_t   datagram_len;
    uint8_t *datagram = read_datagram("dgram/info/css.bin", &datagram_len);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(packet, NULL);

    size_t   response_len;
    uint8_t *response = ssq_packets_to_response((const SSQ_PACKET *const *)(&packet), 1, &response_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(response, NULL);

    cr_expect(!ssq_response_has_challenge(response, response_len));

    free(datagram);

    ssq_packet_free(packet);

    free(response);
}

Test(response, get_challenge) {
    size_t   datagram_len;
    uint8_t *datagram = read_datagram("dgram/chall/example_0.bin", &datagram_len);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(packet, NULL);

    size_t   response_len;
    uint8_t *response = ssq_packets_to_response((const SSQ_PACKET *const *)(&packet), 1, &response_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(response, NULL);

    cr_expect_eq(ssq_response_get_challenge(response, response_len), 0x22D5A14B);

    free(datagram);

    ssq_packet_free(packet);

    free(response);
}

Test(response, is_truncated) {
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

    size_t   response_len;
    uint8_t *response = ssq_packets_to_response((const SSQ_PACKET *const *)packets, packet_count, &response_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(response, NULL);

    cr_expect(ssq_response_is_truncated(response, response_len));

    for (size_t i = 0; i < packet_count; ++i)
        ssq_packet_free(packets[i]);

    free(response);
}

Test(response, is_not_truncated) {
    size_t   datagram_len;
    uint8_t *datagram = read_datagram("dgram/info/css.bin", &datagram_len);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(packet, NULL);

    size_t   response_len;
    uint8_t *response = ssq_packets_to_response((const SSQ_PACKET *const *)(&packet), 1, &response_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(response, NULL);

    cr_expect(!ssq_response_is_truncated(response, response_len));

    free(datagram);

    ssq_packet_free(packet);

    free(response);
}
