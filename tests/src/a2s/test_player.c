#include <criterion/criterion.h>
#include "ssq/a2s/player.h"

A2S_PLAYER *ssq_player_deserialize(
    const char       response[],
    const size_t     response_len,
    uint8_t   *const player_count,
    SSQ_ERROR *const err
);

static void helper_assert_players_eq(
    const A2S_PLAYER *const actual,
    const uint8_t           expected_index,
    const char              expected_name[],
    const int32_t           expected_score,
    const float             expected_duration
) {
    cr_assert_eq(actual->index, expected_index);
    cr_assert_str_eq(actual->name, expected_name);
    cr_assert_eq(actual->name_len, strlen(expected_name));
    cr_assert_eq(actual->score, expected_score);
    cr_assert_float_eq(actual->duration, expected_duration, 1e-6);
}

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

    cr_assert_eq(err.code, SSQ_OK);

    cr_assert_eq(player_count, 2);
    cr_assert_neq(players, NULL);

    helper_assert_players_eq(players + 0, 1, "[D]---->T.N.W<----", 14, 514.370361);
    helper_assert_players_eq(players + 1, 2, "Killer !!!", 5, 434.284454);

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

    cr_assert_eq(err.code, SSQ_ERR_BADRES);
    cr_assert_str_eq(err.message, "Invalid A2S_PLAYER response header");
    cr_assert_eq(players, NULL);
    cr_assert_eq(player_count, 0);
}
