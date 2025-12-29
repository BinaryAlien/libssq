#ifndef SSQ_TIMEOUT_H
#define SSQ_TIMEOUT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef uint32_t ssq_milliseconds;

struct ssq_timeout;

enum ssq_timeout_selector {
    SSQ_TIMEOUT_RECV = 1 << 0,
    SSQ_TIMEOUT_SEND = 1 << 1,
};

void ssq_timeout_set(
    struct ssq_timeout       *timeout,
    enum ssq_timeout_selector which,
    ssq_milliseconds          duration
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_TIMEOUT_H */
