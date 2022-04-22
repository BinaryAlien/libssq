#include <stdlib.h>
#include <string.h>
#include "ssq/a2s/rules.h"
#include "ssq/buf.h"
#include "ssq/query.h"
#include "ssq/response.h"

#define A2S_HEADER_RULES 0x56
#define S2A_HEADER_RULES 0x45

#define A2S_RULES_PAYLOAD_LEN              9
#define A2S_RULES_PAYLOAD_CHALLENGE_OFFSET 5

static const uint8_t g_a2s_rules_payload_template[A2S_RULES_PAYLOAD_LEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, A2S_HEADER_RULES, 0xFF, 0xFF, 0xFF, 0xFF
};

static inline void ssq_rules_payload_init(uint8_t payload[A2S_RULES_PAYLOAD_LEN]) {
    memcpy(payload, g_a2s_rules_payload_template, A2S_RULES_PAYLOAD_LEN);
}

static inline void ssq_rules_payload_set_challenge(uint8_t payload[A2S_RULES_PAYLOAD_LEN], const int32_t chall) {
    memcpy(payload + A2S_RULES_PAYLOAD_CHALLENGE_OFFSET, &chall, sizeof (chall));
}

A2S_RULES *ssq_rules_deserialize(
    const uint8_t    response[],
    const size_t     response_len,
    uint16_t  *const rule_count,
    SSQ_ERROR *const err
) {
    A2S_RULES *rules = NULL;

    SSQ_BUF buf = ssq_buf_init(response, response_len);

    if (ssq_response_is_truncated(response, response_len))
        ssq_buf_forward(&buf, 4);

    const uint8_t response_header = ssq_buf_get_uint8(&buf);

    if (response_header == S2A_HEADER_RULES) {
        *rule_count = ssq_buf_get_uint16(&buf);

        if (*rule_count != 0) {
            rules = calloc(*rule_count, sizeof (*rules));

            if (rules != NULL) {
                for (uint16_t i = 0; i < *rule_count; ++i) {
                    rules[i].name  = ssq_buf_get_string(&buf, &(rules[i].name_len));
                    rules[i].value = ssq_buf_get_string(&buf, &(rules[i].value_len));
                }
            } else {
                ssq_error_set_from_errno(err);
            }
        }
    } else {
        ssq_error_set(err, SSQ_ERR_BADRES, "Invalid A2S_RULES response header");
    }

    return rules;
}

static uint8_t *ssq_rules_query(SSQ_QUERIER *const querier, size_t *const response_len) {
    uint8_t payload[A2S_RULES_PAYLOAD_LEN];
    ssq_rules_payload_init(payload);

    uint8_t *response = ssq_query(querier, payload, A2S_RULES_PAYLOAD_LEN, response_len);

    while (ssq_ok(querier) && ssq_response_has_challenge(response, *response_len)) {
        const int32_t chall = ssq_response_get_challenge(response, *response_len);
        ssq_rules_payload_set_challenge(payload, chall);

        free(response);
        response = ssq_query(querier, payload, A2S_RULES_PAYLOAD_LEN, response_len);
    }

    return response;
}

A2S_RULES *ssq_rules(SSQ_QUERIER *const querier, uint16_t *const rule_count) {
    A2S_RULES *rules = NULL;

    size_t         response_len;
    uint8_t *const response = ssq_rules_query(querier, &response_len);

    if (ssq_ok(querier)) {
        rules = ssq_rules_deserialize(response, response_len, rule_count, &(querier->err));
        free(response);
    }

    return rules;
}

void ssq_rules_free(A2S_RULES rules[], const uint16_t rule_count) {
    for (uint16_t i = 0; i < rule_count; ++i) {
        free(rules[i].name);
        free(rules[i].value);
    }

    free(rules);
}
