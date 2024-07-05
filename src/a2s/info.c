#include "ssq/a2s/info.h"

#include <stdlib.h>
#include <string.h>

#include "ssq/packet.h"
#include "ssq/query.h"
#include "ssq/response.h"
#include "ssq/server/private.h"
#include "ssq/stream.h"

#define A2S_HEADER_INFO 0x54
#define S2A_HEADER_INFO 0x49

#define A2S_INFO_CHALL_LEN    (sizeof (int32_t))
#define A2S_INFO_CHALL_OFFSET (A2S_INFO_PAYLOAD_LEN_WITH_CHALL - A2S_INFO_CHALL_LEN)

#define A2S_INFO_PAYLOAD_LEN_WITH_CHALL    29
#define A2S_INFO_PAYLOAD_LEN_WITHOUT_CHALL (A2S_INFO_PAYLOAD_LEN_WITH_CHALL - A2S_INFO_CHALL_LEN)

static const uint8_t payload_template[A2S_INFO_PAYLOAD_LEN_WITH_CHALL] = {
    0xFF, 0xFF, 0xFF, 0xFF, A2S_HEADER_INFO,
    'S', 'o', 'u', 'r', 'c', 'e', ' ', 'E', 'n', 'g', 'i', 'n', 'e', ' ', 'Q', 'u', 'e', 'r', 'y', '\0',
};

static inline void payload_init(uint8_t payload[A2S_INFO_PAYLOAD_LEN_WITH_CHALL]) {
    memcpy(payload, payload_template, A2S_INFO_PAYLOAD_LEN_WITH_CHALL);
}

static inline void payload_set_challenge(uint8_t payload[A2S_INFO_PAYLOAD_LEN_WITH_CHALL], int32_t chall) {
    memcpy(payload + A2S_INFO_CHALL_OFFSET, &chall, sizeof (chall));
}

static uint8_t *ssq_info_query(SSQ_SERVER *server, size_t *response_len) {
    // Allocate additional storage for possible challenge.
    uint8_t payload[A2S_INFO_PAYLOAD_LEN_WITH_CHALL];
    payload_init(payload);
    uint8_t *response = ssq_query(server, payload, A2S_INFO_PAYLOAD_LEN_WITHOUT_CHALL, response_len);
    while (response != NULL && ssq_response_has_challenge(response, *response_len)) {
        int32_t chall = ssq_response_get_challenge(response, *response_len);
        payload_set_challenge(payload, chall);
        free(response);
        response = ssq_query(server, payload, A2S_INFO_PAYLOAD_LEN_WITH_CHALL, response_len);
    }
    return response;
}

static A2S_ENVIRONMENT ssq_info_deserialize_environment(SSQ_STREAM *stream) {
    switch (ssq_stream_read_uint8_t(stream)) {
        case 'l': return A2S_ENVIRONMENT_LINUX;
        case 'w': return A2S_ENVIRONMENT_WINDOWS;
        case 'm':
        case 'o': return A2S_ENVIRONMENT_MAC;
        default:  return A2S_ENVIRONMENT_UNKNOWN;
    }
}

static A2S_SERVER_TYPE ssq_info_deserialize_server_type(SSQ_STREAM *stream) {
    switch (ssq_stream_read_uint8_t(stream)) {
        case 'd': return A2S_SERVER_TYPE_DEDICATED;
        case 'l': return A2S_SERVER_TYPE_NON_DEDICATED;
        case 'p': return A2S_SERVER_TYPE_STV_RELAY;
        default:  return A2S_SERVER_TYPE_UNKNOWN;
    }
}

A2S_INFO *ssq_info_deserialize(const uint8_t payload[], size_t payload_len, SSQ_ERROR *error) {
    SSQ_STREAM stream;
    ssq_stream_wrap(&stream, payload, payload_len);
    if (ssq_response_is_truncated(payload, payload_len))
        ssq_stream_advance(&stream, SSQ_PACKET_HEADER_LEN);
    uint8_t response_header = ssq_stream_read_uint8_t(&stream);
    if (response_header != S2A_HEADER_INFO) {
        ssq_error_set(error, SSQE_INVALID_RESPONSE, "Invalid A2S_INFO response header");
        return NULL;
    }
    A2S_INFO *info = malloc(sizeof (*info));
    if (info == NULL) {
        ssq_error_set_from_errno(error);
        return NULL;
    }
    memset(info, 0, sizeof (*info));
    info->protocol    = ssq_stream_read_uint8_t(&stream);
    info->name        = ssq_stream_read_string(&stream, &info->name_len);
    info->map         = ssq_stream_read_string(&stream, &info->map_len);
    info->folder      = ssq_stream_read_string(&stream, &info->folder_len);
    info->game        = ssq_stream_read_string(&stream, &info->game_len);
    info->id          = ssq_stream_read_uint16_t(&stream);
    info->players     = ssq_stream_read_uint8_t(&stream);
    info->max_players = ssq_stream_read_uint8_t(&stream);
    info->bots        = ssq_stream_read_uint8_t(&stream);
    info->server_type = ssq_info_deserialize_server_type(&stream);
    info->environment = ssq_info_deserialize_environment(&stream);
    info->visibility  = ssq_stream_read_bool(&stream);
    info->vac         = ssq_stream_read_bool(&stream);
    info->version     = ssq_stream_read_string(&stream, &info->version_len);
    if (ssq_stream_end(&stream))
        return info;
    info->edf = ssq_stream_read_uint8_t(&stream);
    if (info->edf & A2S_INFO_FLAG_PORT)
        info->port = ssq_stream_read_uint16_t(&stream);
    if (info->edf & A2S_INFO_FLAG_STEAMID)
        info->steamid = ssq_stream_read_uint64_t(&stream);
    if (info->edf & A2S_INFO_FLAG_STV) {
        info->stv_port = ssq_stream_read_uint16_t(&stream);
        info->stv_name = ssq_stream_read_string(&stream, &info->stv_name_len);
    }
    if (info->edf & A2S_INFO_FLAG_KEYWORDS)
        info->keywords = ssq_stream_read_string(&stream, &info->keywords_len);
    if (info->edf & A2S_INFO_FLAG_GAMEID)
        info->gameid = ssq_stream_read_uint64_t(&stream);
    return info;
}

A2S_INFO *ssq_info(SSQ_SERVER *server) {
    size_t response_len;
    uint8_t *response = ssq_info_query(server, &response_len);
    if (response == NULL)
        return NULL;
    A2S_INFO *info = ssq_info_deserialize(response, response_len, &server->last_error);
    free(response);
    return info;
}

void ssq_info_free(A2S_INFO *info) {
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

bool ssq_info_has_gameid(const A2S_INFO *info)   { return info->edf & A2S_INFO_FLAG_GAMEID;   }
bool ssq_info_has_keywords(const A2S_INFO *info) { return info->edf & A2S_INFO_FLAG_KEYWORDS; }
bool ssq_info_has_port(const A2S_INFO *info)     { return info->edf & A2S_INFO_FLAG_PORT;     }
bool ssq_info_has_steamid(const A2S_INFO *info)  { return info->edf & A2S_INFO_FLAG_STEAMID;  }
bool ssq_info_has_stv(const A2S_INFO *info)      { return info->edf & A2S_INFO_FLAG_STV;      }
