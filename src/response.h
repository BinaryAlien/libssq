#ifndef SSQ_RESPONSE_H
#define SSQ_RESPONSE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "packet.h"

#define SSQ_RESPONSE_HEADER_CHALLENGE 0x41

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int32_t ssq_response_challenge;
typedef uint8_t ssq_response_header;

struct ssq_response {
  uint8_t *data;
  size_t size;
};

struct ssq_response *
ssq_response_from_packets(const struct ssq_packet *const *packets);
void ssq_response_free(struct ssq_response *response);

bool ssq_response_is_truncated(const struct ssq_response *response);

bool ssq_response_has_challenge(const struct ssq_response *response);
ssq_response_challenge
ssq_response_get_challenge(const struct ssq_response *response);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_RESPONSE_H */
