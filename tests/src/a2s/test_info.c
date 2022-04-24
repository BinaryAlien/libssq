#include <criterion/criterion.h>
#include "helper.h"
#include "ssq/a2s/info.h"
#include "ssq/packet.h"

A2S_INFO *ssq_info_deserialize(const uint8_t *response, size_t response_len, SSQ_ERROR *err);

Test(a2s_info, css) {
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

    A2S_INFO *info = ssq_info_deserialize(response, response_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(info, NULL);

    cr_expect_eq(info->protocol, 2);
    cr_expect_str_eq(info->name, "game2xs.com Counter-Strike Source #1");
    cr_expect_eq(info->name_len, 36);
    cr_expect_str_eq(info->map, "de_dust");
    cr_expect_eq(info->map_len, 7);
    cr_expect_str_eq(info->folder, "cstrike");
    cr_expect_eq(info->folder_len, 7);
    cr_expect_str_eq(info->game, "Counter-Strike: Source");
    cr_expect_eq(info->game_len, 22);
    cr_expect_eq(info->id, 240);
    cr_expect_eq(info->players, 5);
    cr_expect_eq(info->max_players, 16);
    cr_expect_eq(info->bots, 4);
    cr_expect_eq(info->server_type, A2S_SERVER_TYPE_DEDICATED);
    cr_expect_eq(info->environment, A2S_ENVIRONMENT_LINUX);
    cr_expect_eq(info->visibility, false);
    cr_expect_eq(info->vac, false);
    cr_expect_str_eq(info->version, "1.0.0.22");
    cr_expect_eq(info->version_len, 8);
    cr_expect_eq(info->edf, 0);
    cr_expect_eq(info->port, 0);
    cr_expect_eq(info->steamid, 0);
    cr_expect_eq(info->stv_port, 0);
    cr_expect_eq(info->stv_name, NULL);
    cr_expect_eq(info->stv_name_len, 0);
    cr_expect_eq(info->keywords, NULL);
    cr_expect_eq(info->keywords_len, 0);
    cr_expect_eq(info->gameid, 0);

    ssq_info_free(info);
    free(datagram);
    ssq_packet_free(packet);
    free(response);
}

Test(a2s_info, tf2) {
    size_t   datagram_len;
    uint8_t *datagram = read_datagram("dgram/info/tf2.bin", &datagram_len);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *packet = ssq_packet_from_datagram(datagram, datagram_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(packet, NULL);

    size_t   response_len;
    uint8_t *response = ssq_packets_to_response((const SSQ_PACKET *const *)(&packet), 1, &response_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(response, NULL);

    A2S_INFO *info = ssq_info_deserialize(response, response_len, &err);

    cr_assert_eq(err.code, SSQ_OK);
    cr_assert_neq(info, NULL);

    cr_expect_eq(info->protocol, 17);
    cr_expect_str_eq(info->name, "\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01skial.com | PAYLOAD+ | US \xE2\x96\x88\xE2\x96\x88\xE2\x96\x88\xE2\x96\x88");
    cr_expect_eq(info->name_len, 62);
    cr_expect_str_eq(info->map, "pl_badwater_pro_v12_skial");
    cr_expect_eq(info->map_len, 25);
    cr_expect_str_eq(info->folder, "tf");
    cr_expect_eq(info->folder_len, 2);
    cr_expect_str_eq(info->game, "\xE2\x96\x88\xE2\x96\x88 Skial \xE2\x96\x88\xE2\x96\x88");
    cr_expect_eq(info->game_len, 19);
    cr_expect_eq(info->id, 440);
    cr_expect_eq(info->players, 32);
    cr_expect_eq(info->max_players, 32);
    cr_expect_eq(info->bots, 0);
    cr_expect_eq(info->server_type, A2S_SERVER_TYPE_DEDICATED);
    cr_expect_eq(info->environment, A2S_ENVIRONMENT_LINUX);
    cr_expect_eq(info->visibility, false);
    cr_expect_eq(info->vac, true);
    cr_expect_str_eq(info->version, "7182415");
    cr_expect_eq(info->version_len, 7);
    cr_expect_eq(info->edf, 0xB1);
    cr_expect_eq(info->port, 27015);
    cr_expect_eq(info->steamid, 85568392920040218);
    cr_expect_eq(info->stv_port, 0);
    cr_expect_eq(info->stv_name, NULL);
    cr_expect_eq(info->stv_name_len, 0);
    cr_expect_str_eq(info->keywords, "skial,stats,free_items,free_unusuals,alltalk,increased_maxplayers,nocrits,norespawntime,payload");
    cr_expect_eq(info->keywords_len, 95);
    cr_expect_eq(info->gameid, 440);

    ssq_info_free(info);
    free(datagram);
    ssq_packet_free(packet);
    free(response);
}

Test(a2s_info, bad_header) {
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

    A2S_INFO *info = ssq_info_deserialize(response, response_len, &err);

    cr_assert_eq(err.code, SSQ_ERR_BADRES);
    cr_assert_str_eq(err.message, "Invalid A2S_INFO response header");
    cr_assert_eq(info, NULL);

    free(datagram);
    ssq_packet_free(packet);
    free(response);
}
