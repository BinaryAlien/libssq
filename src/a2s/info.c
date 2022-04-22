#include <stdlib.h>
#include <string.h>
#include "ssq/a2s/info.h"
#include "ssq/buf.h"
#include "ssq/query.h"
#include "ssq/response.h"

#define A2S_HEADER_INFO 0x54
#define S2A_HEADER_INFO 0x49

#define A2S_INFO_PAYLOAD_LEN                   29
#define A2S_INFO_PAYLOAD_LEN_WITHOUT_CHALLENGE (A2S_INFO_PAYLOAD_LEN - 4)
#define A2S_INFO_PAYLOAD_CHALLENGE_OFFSET      25

static const uint8_t g_a2s_info_payload_template[A2S_INFO_PAYLOAD_LEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, A2S_HEADER_INFO,
    0x53, 0x6F, 0x75, 0x72, 0x63,
    0x65, 0x20, 0x45, 0x6E, 0x67,
    0x69, 0x6E, 0x65, 0x20, 0x51,
    0x75, 0x65, 0x72, 0x79, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF
};

static inline void ssq_info_payload_init(uint8_t payload[A2S_INFO_PAYLOAD_LEN]) {
    memcpy(payload, g_a2s_info_payload_template, A2S_INFO_PAYLOAD_LEN);
}

static inline void ssq_info_payload_set_challenge(uint8_t payload[A2S_INFO_PAYLOAD_LEN], const int32_t chall) {
    memcpy(payload + A2S_INFO_PAYLOAD_CHALLENGE_OFFSET, &chall, sizeof (chall));
}

static A2S_ENVIRONMENT ssq_info_deserialize_environment(SSQ_BUF *const buf) {
    switch (ssq_buf_get_uint8(buf)) {
        case 'l': return A2S_ENVIRONMENT_LINUX;
        case 'w': return A2S_ENVIRONMENT_WINDOWS;
        case 'm':
        case 'o': return A2S_ENVIRONMENT_MAC;
        default:  return A2S_ENVIRONMENT_UNKNOWN;
    }
}

static A2S_SERVER_TYPE ssq_info_deserialize_server_type(SSQ_BUF *const buf) {
    switch (ssq_buf_get_uint8(buf)) {
        case 'd': return A2S_SERVER_TYPE_DEDICATED;
        case 'l': return A2S_SERVER_TYPE_NON_DEDICATED;
        case 'p': return A2S_SERVER_TYPE_STV_RELAY;
        default:  return A2S_SERVER_TYPE_UNKNOWN;
    }
}

A2S_INFO *ssq_info_deserialize(const uint8_t payload[], const size_t payload_len, SSQ_ERROR *const err) {
    A2S_INFO *info = NULL;

    SSQ_BUF buf = ssq_buf_init(payload, payload_len);

    if (ssq_response_is_truncated(payload, payload_len))
        ssq_buf_forward(&buf, 4);

    const uint8_t response_header = ssq_buf_get_uint8(&buf);

    if (response_header == S2A_HEADER_INFO) {
        info = malloc(sizeof (*info));

        if (info != NULL) {
            memset(info, 0, sizeof (*info));

            info->protocol    = ssq_buf_get_uint8(&buf);
            info->name        = ssq_buf_get_string(&buf, &(info->name_len));
            info->map         = ssq_buf_get_string(&buf, &(info->map_len));
            info->folder      = ssq_buf_get_string(&buf, &(info->folder_len));
            info->game        = ssq_buf_get_string(&buf, &(info->game_len));
            info->id          = ssq_buf_get_uint16(&buf);
            info->players     = ssq_buf_get_uint8(&buf);
            info->max_players = ssq_buf_get_uint8(&buf);
            info->bots        = ssq_buf_get_uint8(&buf);
            info->server_type = ssq_info_deserialize_server_type(&buf);
            info->environment = ssq_info_deserialize_environment(&buf);
            info->visibility  = ssq_buf_get_bool(&buf);
            info->vac         = ssq_buf_get_bool(&buf);
            info->version     = ssq_buf_get_string(&buf, &(info->version_len));

            if (!ssq_buf_eof(&buf)) {
                info->edf = ssq_buf_get_uint8(&buf);

                if (info->edf & A2S_INFO_FLAG_PORT)
                    info->port = ssq_buf_get_uint16(&buf);

                if (info->edf & A2S_INFO_FLAG_STEAMID)
                    info->steamid = ssq_buf_get_uint64(&buf);

                if (info->edf & A2S_INFO_FLAG_STV) {
                    info->stv_port = ssq_buf_get_uint16(&buf);
                    info->stv_name = ssq_buf_get_string(&buf, &(info->stv_name_len));
                }

                if (info->edf & A2S_INFO_FLAG_KEYWORDS)
                    info->keywords = ssq_buf_get_string(&buf, &(info->keywords_len));

                if (info->edf & A2S_INFO_FLAG_GAMEID)
                    info->gameid = ssq_buf_get_uint64(&buf);
            }
        } else {
            ssq_error_set_from_errno(err);
        }
    } else {
        ssq_error_set(err, SSQ_ERR_BADRES, "Invalid A2S_INFO response header");
    }

    return info;
}

static uint8_t *ssq_info_query(SSQ_QUERIER *const querier, size_t *const response_len) {
    uint8_t payload[A2S_INFO_PAYLOAD_LEN];
    ssq_info_payload_init(payload);

    uint8_t *response = ssq_query(querier, payload, A2S_INFO_PAYLOAD_LEN_WITHOUT_CHALLENGE, response_len);

    while (ssq_ok(querier) && ssq_response_has_challenge(response, *response_len)) {
        const int32_t chall = ssq_response_get_challenge(response, *response_len);
        ssq_info_payload_set_challenge(payload, chall);

        free(response);
        response = ssq_query(querier, payload, A2S_INFO_PAYLOAD_LEN, response_len);
    }

    return response;
}

A2S_INFO *ssq_info(SSQ_QUERIER *const querier) {
    A2S_INFO *info = NULL;

    size_t         response_len;
    uint8_t *const response = ssq_info_query(querier, &response_len);

    if (ssq_ok(querier)) {
        info = ssq_info_deserialize(response, response_len, &(querier->err));
        free(response);
    }

    return info;
}

void ssq_info_free(A2S_INFO *const info) {
    free(info->name);
    free(info->map);
    free(info->folder);
    free(info->game);
    free(info->version);

    if (ssq_info_has_stv(info))
        free(info->stv_name);

    if (ssq_info_has_keywords(info))
        free(info->keywords);

    free(info);
}
