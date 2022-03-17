#include <criterion/criterion.h>
#include "ssq/a2s/player.h"

A2S_PLAYER *ssq_player_deserialize(
    const char       response[],
    const size_t     response_len,
    uint8_t   *const player_count,
    SSQ_ERROR *const err
);

Test(a2s_player, wiki_example, .description="Wiki Example") {
    const char response[] = {
        0x44, 0x02, 0x01, 0x5B, 0x44, 0x5D, 0x2D, 0x2D, 0x2D, 0x2D, 0x3E, 0x54, 0x2E, 0x4E, 0x2E, 0x57,
        0x3C, 0x2D, 0x2D, 0x2D, 0x2D, 0x00, 0x0E, 0x00, 0x00, 0x00, 0xB4, 0x97, 0x00, 0x44, 0x02, 0x4B,
        0x69, 0x6C, 0x6C, 0x65, 0x72, 0x20, 0x21, 0x21, 0x21, 0x00, 0x05, 0x00, 0x00, 0x00, 0x69, 0x24,
        0xD9, 0x43
    };

    const size_t response_len = sizeof (response);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    uint8_t           player_count = 0;
    A2S_PLAYER *const players      = ssq_player_deserialize(response, response_len, &player_count, &err);

    cr_assert(err.code == SSQ_OK);

    cr_assert(player_count == 2);
    cr_assert(players != NULL);

    cr_assert(players[0].index == 1);
    cr_assert_str_eq(players[0].name, "[D]---->T.N.W<----");
    cr_assert(players[0].name_len == 18);
    cr_assert(players[0].score == 14);
    cr_assert_float_eq(players[0].duration, 514.370361, 1e-6);

    cr_assert(players[1].index == 2);
    cr_assert_str_eq(players[1].name, "Killer !!!");
    cr_assert(players[1].name_len == 10);
    cr_assert(players[1].score == 5);
    cr_assert_float_eq(players[1].duration, 434.284454, 1e-6);

    ssq_player_free(players, player_count);
}

Test(a2s_player, bad_header, .description="Invalid response header") {
    const uint8_t bad_header = 0x00;

    const char response[] = {
        bad_header, 0x02, 0x01, 0x5B, 0x44, 0x5D, 0x2D, 0x2D, 0x2D, 0x2D, 0x3E, 0x54, 0x2E, 0x4E, 0x2E, 0x57,
        0x3C, 0x2D, 0x2D, 0x2D, 0x2D, 0x00, 0x0E, 0x00, 0x00, 0x00, 0xB4, 0x97, 0x00, 0x44, 0x02, 0x4B,
        0x69, 0x6C, 0x6C, 0x65, 0x72, 0x20, 0x21, 0x21, 0x21, 0x00, 0x05, 0x00, 0x00, 0x00, 0x69, 0x24,
        0xD9, 0x43
    };

    const size_t response_len = sizeof (response);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    uint8_t           player_count = 0;
    A2S_PLAYER *const players      = ssq_player_deserialize(response, response_len, &player_count, &err);

    cr_assert(err.code == SSQ_ERR_BADRES);
    cr_assert_str_eq(err.message, "Invalid A2S_PLAYER response header");
    cr_assert(players == NULL);
    cr_assert(player_count == 0);
}
