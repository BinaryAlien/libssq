#include "ssq/a2s/player.h"

#include <stdlib.h>
#include <string.h>

#include "ssq/query.h"
#include "ssq/response.h"
#include "ssq/server/private.h"
#include "ssq/stream.h"

#define A2S_HEADER_PLAYER 0x55
#define S2A_HEADER_PLAYER 0x44

#define A2S_PLAYER_PAYLOAD_LEN              9
#define A2S_PLAYER_PAYLOAD_CHALLENGE_OFFSET (A2S_PLAYER_PAYLOAD_LEN - 4)

static const uint8_t payload_template[A2S_PLAYER_PAYLOAD_LEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, A2S_HEADER_PLAYER, 0xFF, 0xFF, 0xFF, 0xFF
};

static inline void payload_init(uint8_t payload[A2S_PLAYER_PAYLOAD_LEN]) {
    memcpy(payload, payload_template, A2S_PLAYER_PAYLOAD_LEN);
}

static inline void payload_set_challenge(uint8_t payload[A2S_PLAYER_PAYLOAD_LEN], int32_t chall) {
    memcpy(payload + A2S_PLAYER_PAYLOAD_CHALLENGE_OFFSET, &chall, sizeof (chall));
}

static uint8_t *ssq_player_query(SSQ_SERVER *server, size_t *response_len) {
    uint8_t payload[A2S_PLAYER_PAYLOAD_LEN];
    payload_init(payload);
    uint8_t *response = ssq_query(server, payload, A2S_PLAYER_PAYLOAD_LEN, response_len);
    while (response != NULL && ssq_response_has_challenge(response, *response_len)) {
        int32_t chall = ssq_response_get_challenge(response, *response_len);
        payload_set_challenge(payload, chall);
        free(response);
        response = ssq_query(server, payload, A2S_PLAYER_PAYLOAD_LEN, response_len);
    }
    return response;
}

static void ssq_player_deserialize_from_stream(SSQ_STREAM *src, A2S_PLAYER *dest) {
    dest->index    = ssq_stream_read_uint8_t(src);
    dest->name     = ssq_stream_read_string(src, &dest->name_len);
    dest->score    = ssq_stream_read_int32_t(src);
    dest->duration = ssq_stream_read_float(src);
}

A2S_PLAYER *ssq_player_deserialize(const uint8_t response[], size_t response_len, uint8_t *player_count, SSQ_ERROR *err) {
    SSQ_STREAM stream;
    ssq_stream_wrap(&stream, response, response_len);
    if (ssq_response_is_truncated(response, response_len))
        ssq_stream_advance(&stream, 4);
    uint8_t response_header = ssq_stream_read_uint8_t(&stream);
    if (response_header != S2A_HEADER_PLAYER) {
        ssq_error_set(err, SSQE_INVALID_RESPONSE, "Invalid A2S_PLAYER response header");
        return NULL;
    }
    *player_count = ssq_stream_read_uint8_t(&stream);
    if (*player_count == 0)
        return NULL;
    A2S_PLAYER *players = calloc(*player_count, sizeof (*players));
    if (players == NULL) {
        ssq_error_set_from_errno(err);
        return NULL;
    }
    for (uint8_t i = 0; i < *player_count; ++i)
        ssq_player_deserialize_from_stream(&stream, players + i);
    return players;
}

A2S_PLAYER *ssq_player(SSQ_SERVER *server, uint8_t *player_count) {
    size_t response_len;
    uint8_t *response = ssq_player_query(server, &response_len);
    if (response == NULL)
        return NULL;
    A2S_PLAYER *players = ssq_player_deserialize(response, response_len, player_count, &server->last_error);
    free(response);
    return players;
}

void ssq_player_free(A2S_PLAYER players[], uint8_t player_count) {
    for (uint8_t i = 0; i < player_count; ++i)
        free(players[i].name);
    free(players);
}
