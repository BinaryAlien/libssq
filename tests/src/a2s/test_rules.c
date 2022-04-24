#include <criterion/criterion.h>
#include "helper.h"
#include "ssq/a2s/rules.h"
#include "ssq/packet.h"

A2S_RULES *ssq_rules_deserialize(const uint8_t *response, size_t response_len, uint16_t *rule_count, SSQ_ERROR *err);

Test(a2s_rules, wiki_example) {
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

    size_t   response_len = 0;
    uint8_t *response     = ssq_packets_to_response((const SSQ_PACKET *const *)packets, packet_count, &response_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(response, NULL);

    uint16_t   rule_count = 0;
    A2S_RULES *rules      = ssq_rules_deserialize(response, response_len, &rule_count, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(rules, NULL);
    cr_assert_eq(rule_count, 224);

    cr_expect_str_eq(rules[0].name, "brimmunity_version");
    cr_expect_eq(rules[0].name_len, 18);
    cr_expect_str_eq(rules[0].value, "1.1.1p");
    cr_expect_eq(rules[0].value_len, 6);

    // ...

    cr_expect_str_eq(rules[223].name, "tv_relaypassword");
    cr_expect_eq(rules[223].name_len, 16);
    cr_expect_str_eq(rules[223].value, "0");
    cr_expect_eq(rules[223].value_len, 1);

    for (size_t i = 0; i < packet_count; ++i)
        ssq_packet_free(packets[i]);

    free(response);
    ssq_rules_free(rules, rule_count);
}

Test(a2s_rules, bad_header) {
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

    uint16_t   rule_count = 0;
    A2S_RULES *rules      = ssq_rules_deserialize(response, response_len, &rule_count, &err);

    cr_assert_eq(err.code, SSQ_ERR_BADRES);
    cr_assert_str_eq(err.message, "Invalid A2S_RULES response header");
    cr_assert_eq(rules, NULL);

    free(datagram);
    ssq_packet_free(packet);
    free(response);
}
