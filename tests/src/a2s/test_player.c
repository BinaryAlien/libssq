#include <criterion/criterion.h>
#include "helper.h"
#include "ssq/a2s/player.h"
#include "ssq/packet.h"

A2S_PLAYER *ssq_player_deserialize(const uint8_t *response, size_t response_len, uint8_t *player_count, SSQ_ERROR *err);

Test(a2s_player, example_0) {
    size_t   datagram_len;
    uint8_t *datagram = read_datagram("dgram/player/example_0.bin", &datagram_len);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(packet, NULL);

    size_t   response_len;
    uint8_t *response = ssq_packets_to_response((const SSQ_PACKET *const *)(&packet), 1, &response_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(response, NULL);

    uint8_t     player_count = 0;
    A2S_PLAYER *players      = ssq_player_deserialize(response, response_len, &player_count, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(players, NULL);
    cr_assert_eq(player_count, 2);

    cr_expect_eq(players[0].index, 1);
    cr_expect_str_eq(players[0].name, "[D]---->T.N.W<----");
    cr_expect_eq(players[0].name_len, 18);
    cr_expect_eq(players[0].score, 14);
    cr_expect_float_eq(players[0].duration, 514.370361F, 1e-6);

    cr_expect_eq(players[1].index, 2);
    cr_expect_str_eq(players[1].name, "Killer !!!");
    cr_expect_eq(players[1].name_len, 10);
    cr_expect_eq(players[1].score, 5);
    cr_expect_float_eq(players[1].duration, 434.284454F, 1e-6);

    free(datagram);
    ssq_packet_free(packet);
    free(response);
    ssq_player_free(players, player_count);
}

Test(a2s_player, bad_header) {
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

    uint8_t     player_count = 0;
    A2S_PLAYER *players      = ssq_player_deserialize(response, response_len, &player_count, &err);

    cr_assert_eq(err.code, SSQ_ERR_BADRES);
    cr_assert_str_eq(err.message, "Invalid A2S_PLAYER response header");
    cr_assert_eq(players, NULL);

    free(datagram);
    ssq_packet_free(packet);
    free(response);
}
