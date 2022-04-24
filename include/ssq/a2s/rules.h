#ifndef SSQ_A2S_RULES_H
#define SSQ_A2S_RULES_H

#include "ssq/ssq.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct a2s_rules {
    char  *name;      /** Name of the rule             */
    size_t name_len;  /** Length of the `name' string  */
    char  *value;     /** Value of the rule            */
    size_t value_len; /** Length of the `value' string */
} A2S_RULES;

/**
 * Sends an A2S_RULES query to a Source game server.
 *
 * @param querier    Source server querier to use
 * @param rule_count where to store the number of rules in the output array
 *
 * @return dynamically-allocated array of `A2S_RULES' structs containing the
 *         rules the server is using, or NULL if an error occurred
 */
A2S_RULES *ssq_rules(SSQ_QUERIER *querier, uint16_t *rule_count);

/**
 * Frees an `A2S_RULES' array.
 *
 * @param rules      `A2S_RULES' array to free
 * @param rule_count number of rules in the `A2S_RULES' array
 */
void ssq_rules_free(A2S_RULES *rules, uint16_t rule_count);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_A2S_RULES_H */
