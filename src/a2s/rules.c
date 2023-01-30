#include "ssq/a2s/rules.h"

#include <stdlib.h>
#include <string.h>

#include "ssq/buf.h"
#include "ssq/query.h"
#include "ssq/response.h"
#include "ssq/server/private.h"

#define A2S_HEADER_RULES 0x56
#define S2A_HEADER_RULES 0x45

#define A2S_RULES_PAYLOAD_LEN              9
#define A2S_RULES_PAYLOAD_CHALLENGE_OFFSET (A2S_RULES_PAYLOAD_LEN - 4)

static const uint8_t g_a2s_rules_payload_template[A2S_RULES_PAYLOAD_LEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, A2S_HEADER_RULES, 0xFF, 0xFF, 0xFF, 0xFF
};

static inline void ssq_rules_payload_init(uint8_t payload[A2S_RULES_PAYLOAD_LEN]) {
    memcpy(payload, g_a2s_rules_payload_template, A2S_RULES_PAYLOAD_LEN);
}

static inline void ssq_rules_payload_set_challenge(uint8_t payload[A2S_RULES_PAYLOAD_LEN], int32_t chall) {
    memcpy(payload + A2S_RULES_PAYLOAD_CHALLENGE_OFFSET, &chall, sizeof (chall));
}

static uint8_t *ssq_rules_query(SSQ_SERVER *server, size_t *response_len) {
    uint8_t payload[A2S_RULES_PAYLOAD_LEN];
    ssq_rules_payload_init(payload);
    uint8_t *response = ssq_query(server, payload, A2S_RULES_PAYLOAD_LEN, response_len);
    while (ssq_server_ok(server) && ssq_response_has_challenge(response, *response_len)) {
        int32_t chall = ssq_response_get_challenge(response, *response_len);
        ssq_rules_payload_set_challenge(payload, chall);
        free(response);
        response = ssq_query(server, payload, A2S_RULES_PAYLOAD_LEN, response_len);
    }
    return response;
}

static void ssq_rules_deserialize_from_buf(SSQ_BUF *src, A2S_RULES *dest) {
    dest->name  = ssq_buf_read_string(src, &dest->name_len);
    dest->value = ssq_buf_read_string(src, &dest->value_len);
}

A2S_RULES *ssq_rules_deserialize(const uint8_t response[], size_t response_len, uint16_t *rule_count, SSQ_ERROR *err) {
    A2S_RULES *rules = NULL;
    SSQ_BUF buf;
    ssq_buf_init(&buf, response, response_len);
    if (ssq_response_is_truncated(response, response_len))
        ssq_buf_forward(&buf, 4);
    uint8_t response_header = ssq_buf_read_uint8(&buf);
    if (response_header == S2A_HEADER_RULES) {
        *rule_count = ssq_buf_read_uint16(&buf);
        if (*rule_count != 0) {
            rules = calloc(*rule_count, sizeof (*rules));
            if (rules != NULL) {
                for (uint16_t i = 0; i < *rule_count; ++i) {
                    ssq_rules_deserialize_from_buf(&buf, rules + i);
                }
            } else {
                ssq_error_set_from_errno(err);
            }
        }
    } else {
        ssq_error_set(err, SSQE_INVALID_RESPONSE, "Invalid A2S_RULES response header");
    }
    return rules;
}

A2S_RULES *ssq_rules(SSQ_SERVER *server, uint16_t *rule_count) {
    A2S_RULES *rules = NULL;
    size_t response_len;
    uint8_t *response = ssq_rules_query(server, &response_len);
    if (ssq_server_ok(server)) {
        rules = ssq_rules_deserialize(response, response_len, rule_count, &server->last_error);
        free(response);
    }
    return rules;
}

void ssq_rules_free(A2S_RULES rules[], uint16_t rule_count) {
    for (uint16_t i = 0; i < rule_count; ++i) {
        free(rules[i].name);
        free(rules[i].value);
    }
    free(rules);
}
