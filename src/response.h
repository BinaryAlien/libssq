#ifndef SSQ_RESPONSE_H
#define SSQ_RESPONSE_H

#include <stdbool.h>

#include "buffer.h"

#define SSQ_RESPONSE_HEADER_CHALLENGE 0x41

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool ssq_response_is_truncated(const struct ssq_buffer *response);

int32_t ssq_response_get_challenge(const struct ssq_buffer *response);

bool ssq_response_has_challenge(const struct ssq_buffer *response);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_RESPONSE_H */
