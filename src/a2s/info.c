#include <stdlib.h>
#include <string.h>
#include "ssq/a2s/info.h"
#include "ssq/buf.h"
#include "ssq/query.h"
#include "ssq/response.h"

#define A2S_INFO_PAYLOAD_LEN            25
#define A2S_INFO_PAYLOAD_LEN_WITH_CHALL (A2S_INFO_PAYLOAD_LEN + sizeof (int32_t))

#define A2S_HEADER_INFO 0x54
#define S2A_HEADER_INFO 0x49

const uint8_t g_a2s_info_payload[A2S_INFO_PAYLOAD_LEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, A2S_HEADER_INFO,
    'S', 'o', 'u', 'r', 'c',
    'e', ' ', 'E', 'n', 'g',
    'i', 'n', 'e', ' ', 'Q',
    'u', 'e', 'r', 'y', '\0'
};

static inline void ssq_info_payload_init(char payload[]) {
    memcpy(payload, g_a2s_info_payload, A2S_INFO_PAYLOAD_LEN);
}

static inline void ssq_info_payload_setchall(char payload[], const int32_t chall) {
    memcpy(payload + A2S_INFO_PAYLOAD_LEN, &chall, sizeof (chall));
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

static A2S_SERVER_TYPE ssq_info_deserialize_servertype(SSQ_BUF *const buf) {
    switch (ssq_buf_get_uint8(buf)) {
        case 'd': return A2S_SERVER_TYPE_DEDICATED;
        case 'l': return A2S_SERVER_TYPE_NON_DEDICATED;
        case 'p': return A2S_SERVER_TYPE_STV_RELAY;
        default:  return A2S_SERVER_TYPE_UNKNOWN;
    }
}

A2S_INFO *ssq_info_deserialize(const char response[], const size_t response_len, SSQ_ERROR *const err) {
    SSQ_BUF buf = ssq_buf_init(response, response_len);

    if (ssq_response_istruncated(response, response_len))
        ssq_buf_forward(&buf, sizeof (int32_t));

    const uint8_t response_header = ssq_buf_get_uint8(&buf);

    A2S_INFO *info = NULL;

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
            info->server_type = ssq_info_deserialize_servertype(&buf);
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
            ssq_error_set_sys(err);
        }
    } else {
        ssq_error_set(err, SSQ_ERR_BADRES, "Invalid A2S_INFO response header");
    }

    return info;
}

A2S_INFO *ssq_info(SSQ_QUERIER *const querier) {
    char payload[A2S_INFO_PAYLOAD_LEN_WITH_CHALL];
    ssq_info_payload_init(payload);

    size_t response_len;
    char  *response = ssq_query(querier, payload, A2S_INFO_PAYLOAD_LEN, &response_len);

    while (ssq_ok(querier) && ssq_response_haschall(response, response_len)) {
        const int32_t query_chall = ssq_response_getchall(response, response_len);
        ssq_info_payload_setchall(payload, query_chall);

        free(response);
        response = ssq_query(querier, payload, A2S_INFO_PAYLOAD_LEN_WITH_CHALL, &response_len);
    }

    A2S_INFO *info = NULL;

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

    if (info->edf & A2S_INFO_FLAG_STV)
        free(info->stv_name);

    if (info->edf & A2S_INFO_FLAG_KEYWORDS)
        free(info->keywords);

    free(info);
}
