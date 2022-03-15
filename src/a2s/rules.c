#include <stdlib.h>
#include <string.h>
#include "ssq/a2s/rules.h"
#include "ssq/buf.h"
#include "ssq/query.h"
#include "ssq/response.h"

#define A2S_RULES_PAYLOAD_LEN 5
#define A2S_RULES_PAYLOAD_LEN_WITH_CHALL (A2S_RULES_PAYLOAD_LEN + sizeof (int32_t))

#define A2S_HEADER_RULES 0x56
#define S2A_HEADER_RULES 0x45

#define A2S_RULES_NOCHALL 0xFFFFFFFF

const uint8_t g_a2s_rules_payload[A2S_RULES_PAYLOAD_LEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, A2S_HEADER_RULES
};

static inline void ssq_rules_payload_init(char payload[]) {
    memcpy(payload, g_a2s_rules_payload, A2S_RULES_PAYLOAD_LEN);
}

static inline void ssq_rules_payload_setchall(char payload[], const int32_t chall) {
    memcpy(payload + A2S_RULES_PAYLOAD_LEN, &chall, sizeof (chall));
}

A2S_RULES *ssq_rules_deserialize(
    const char       response[],
    const size_t     response_len,
    uint16_t  *const rule_count,
    SSQ_ERROR *const err
) {
    A2S_RULES *rules = NULL;

    SSQ_BUF buf = ssq_buf_init(response, response_len);

    if (ssq_response_istruncated(response, response_len))
        ssq_buf_forward(&buf, sizeof (int32_t));

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
                ssq_error_set_sys(err);
            }
        }
    } else {
        ssq_error_set(err, SSQ_ERR_BADRES, "Invalid A2S_RULES response_header");
    }

    return rules;
}

A2S_RULES *ssq_rules(SSQ_QUERIER *const querier, uint16_t *const rule_count) {
    char query_payload[A2S_RULES_PAYLOAD_LEN_WITH_CHALL];
    ssq_rules_payload_init(query_payload);
    ssq_rules_payload_setchall(query_payload, A2S_RULES_NOCHALL);

    size_t response_len;
    char  *response = ssq_query(querier, query_payload, A2S_RULES_PAYLOAD_LEN_WITH_CHALL, &response_len);

    while (ssq_ok(querier) && ssq_response_haschall(response, response_len)) {
        const int32_t query_chall = ssq_response_getchall(response, response_len);
        ssq_rules_payload_setchall(query_payload, query_chall);

        free(response);
        response = ssq_query(querier, query_payload, A2S_RULES_PAYLOAD_LEN_WITH_CHALL, &response_len);
    }

    A2S_RULES *rules = NULL;

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
