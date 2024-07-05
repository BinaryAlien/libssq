#include "ssq/a2s/rules.h"

#include <stdlib.h>
#include <string.h>

#include "ssq/packet.h"
#include "ssq/query.h"
#include "ssq/response.h"
#include "ssq/server/private.h"
#include "ssq/stream.h"

#define A2S_HEADER_RULES 0x56
#define S2A_HEADER_RULES 0x45

#define A2S_RULES_CHALL_LEN    (sizeof (int32_t))
#define A2S_RULES_CHALL_OFFSET (A2S_RULES_PAYLOAD_LEN - A2S_RULES_CHALL_LEN)

#define A2S_RULES_PAYLOAD_LEN 9

static const uint8_t payload_template[A2S_RULES_PAYLOAD_LEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, A2S_HEADER_RULES, 0xFF, 0xFF, 0xFF, 0xFF,
};

static inline void payload_init(uint8_t payload[A2S_RULES_PAYLOAD_LEN]) {
    memcpy(payload, payload_template, A2S_RULES_PAYLOAD_LEN);
}

static inline void payload_set_challenge(uint8_t payload[A2S_RULES_PAYLOAD_LEN], int32_t chall) {
    memcpy(payload + A2S_RULES_CHALL_OFFSET, &chall, sizeof (chall));
}

static uint8_t *ssq_rules_query(SSQ_SERVER *server, size_t *response_len) {
    uint8_t payload[A2S_RULES_PAYLOAD_LEN];
    payload_init(payload);
    uint8_t *response = ssq_query(server, payload, A2S_RULES_PAYLOAD_LEN, response_len);
    while (response != NULL && ssq_response_has_challenge(response, *response_len)) {
        int32_t chall = ssq_response_get_challenge(response, *response_len);
        payload_set_challenge(payload, chall);
        free(response);
        response = ssq_query(server, payload, A2S_RULES_PAYLOAD_LEN, response_len);
    }
    return response;
}

static void ssq_rules_deserialize_from_stream(SSQ_STREAM *src, A2S_RULES *dest) {
    dest->name  = ssq_stream_read_string(src, &dest->name_len);
    dest->value = ssq_stream_read_string(src, &dest->value_len);
}

A2S_RULES *ssq_rules_deserialize(const uint8_t response[], size_t response_len, uint16_t *rule_count, SSQ_ERROR *error) {
    SSQ_STREAM stream;
    ssq_stream_wrap(&stream, response, response_len);
    if (ssq_response_is_truncated(response, response_len))
        ssq_stream_advance(&stream, SSQ_PACKET_HEADER_LEN);
    uint8_t response_header = ssq_stream_read_uint8_t(&stream);
    if (response_header != S2A_HEADER_RULES) {
        ssq_error_set(error, SSQE_INVALID_RESPONSE, "Invalid A2S_RULES response header");
        return NULL;
    }
    *rule_count = ssq_stream_read_uint16_t(&stream);
    if (*rule_count == 0)
        return NULL;
    A2S_RULES *rules = calloc(*rule_count, sizeof (*rules));
    if (rules == NULL) {
        ssq_error_set_from_errno(error);
        return NULL;
    }
    for (uint16_t i = 0; i < *rule_count; ++i)
        ssq_rules_deserialize_from_stream(&stream, rules + i);
    return rules;
}

A2S_RULES *ssq_rules(SSQ_SERVER *server, uint16_t *rule_count) {
    size_t response_len;
    uint8_t *response = ssq_rules_query(server, &response_len);
    if (response == NULL)
        return NULL;
    A2S_RULES *rules = ssq_rules_deserialize(response, response_len, rule_count, &server->last_error);
    free(response);
    return rules;
}

void ssq_rules_free(A2S_RULES rules[], uint16_t rule_count) {
    for (uint16_t i = 0; i < rule_count; ++i) {
        free(rules[i].name);
        free(rules[i].value);
    }
    free(rules);
}
