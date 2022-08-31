#include "ssq/a2s/player.h"

#include <stdlib.h>
#include <string.h>

#include "ssq/buf.h"
#include "ssq/query.h"
#include "ssq/response.h"

#define A2S_HEADER_PLAYER 0x55
#define S2A_HEADER_PLAYER 0x44

#define A2S_PLAYER_PAYLOAD_LEN              9
#define A2S_PLAYER_PAYLOAD_CHALLENGE_OFFSET (A2S_PLAYER_PAYLOAD_LEN - 4)

static const uint8_t a2s_player_payload_template[A2S_PLAYER_PAYLOAD_LEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, A2S_HEADER_PLAYER, 0xFF, 0xFF, 0xFF, 0xFF
};

static inline void ssq_player_payload_init(uint8_t payload[A2S_PLAYER_PAYLOAD_LEN]) {
    memcpy(payload, a2s_player_payload_template, A2S_PLAYER_PAYLOAD_LEN);
}

static inline void ssq_player_payload_set_challenge(uint8_t payload[A2S_PLAYER_PAYLOAD_LEN], int32_t chall) {
    memcpy(payload + A2S_PLAYER_PAYLOAD_CHALLENGE_OFFSET, &chall, sizeof (chall));
}

static uint8_t *ssq_player_query(SSQ_SERVER *server, size_t *response_len) {
    uint8_t payload[A2S_PLAYER_PAYLOAD_LEN];
    ssq_player_payload_init(payload);
    uint8_t *response = ssq_query(server, payload, A2S_PLAYER_PAYLOAD_LEN, response_len);
    while (ssq_server_ok(server) && ssq_response_has_challenge(response, *response_len)) {
        int32_t chall = ssq_response_get_challenge(response, *response_len);
        ssq_player_payload_set_challenge(payload, chall);
        free(response);
        response = ssq_query(server, payload, A2S_PLAYER_PAYLOAD_LEN, response_len);
    }
    return response;
}

static void ssq_player_deserialize_from_buf(SSQ_BUF *src, A2S_PLAYER *dest) {
    dest->index    = ssq_buf_read_uint8(src);
    dest->name     = ssq_buf_read_string(src, &(dest->name_len));
    dest->score    = ssq_buf_read_int32(src);
    dest->duration = ssq_buf_read_float(src);
}

A2S_PLAYER *ssq_player_deserialize(const uint8_t response[], size_t response_len, uint8_t *player_count, SSQ_ERROR *err) {
    A2S_PLAYER *players = NULL;
    SSQ_BUF buf;
    ssq_buf_init(&buf, response, response_len);
    if (ssq_response_is_truncated(response, response_len))
        ssq_buf_forward(&buf, 4);
    uint8_t response_header = ssq_buf_read_uint8(&buf);
    if (response_header == S2A_HEADER_PLAYER) {
        *player_count = ssq_buf_read_uint8(&buf);
        if (*player_count != 0) {
            players = calloc(*player_count, sizeof (*players));
            if (players != NULL) {
                for (uint8_t i = 0; i < *player_count; ++i) {
                    ssq_player_deserialize_from_buf(&buf, players + i);
                }
            } else {
                ssq_error_set_from_errno(err);
            }
        }
    } else {
        ssq_error_set(err, SSQE_INVALID_RESPONSE, "Invalid A2S_PLAYER response header");
    }
    return players;
}

A2S_PLAYER *ssq_player(SSQ_SERVER *server, uint8_t *player_count) {
    A2S_PLAYER *players = NULL;
    size_t response_len;
    uint8_t *response = ssq_player_query(server, &response_len);
    if (ssq_server_ok(server)) {
        players = ssq_player_deserialize(response, response_len, player_count, &(server->last_error));
        free(response);
    }
    return players;
}

void ssq_player_free(A2S_PLAYER players[], uint8_t player_count) {
    for (uint8_t i = 0; i < player_count; ++i)
        free(players[i].name);
    free(players);
}
