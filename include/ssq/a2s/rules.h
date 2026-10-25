#ifndef SSQ_A2S_RULES_H
#define SSQ_A2S_RULES_H

#include <stddef.h>

#include "ssq/server.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_a2s_rule {
  /* Name of the rule. */
  char *name;
  /* Size in bytes of the `rule` UTF-8 string. */
  size_t name_size;

  /* Value of the rule. */
  char *value;
  /* Size in bytes of the `value` UTF-8 string. */
  size_t value_size;
};

struct ssq_a2s_rule *ssq_a2s_rules(struct ssq_server *server, uint16_t *count);

void ssq_a2s_rules_free(struct ssq_a2s_rule *rules, uint16_t count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_A2S_RULES_H */
