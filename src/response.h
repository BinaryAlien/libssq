#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define S2A_HEADER_CHALL 0x41

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool    ssq_response_has_challenge(const uint8_t *response, size_t response_len);
int32_t ssq_response_get_challenge(const uint8_t *response, size_t response_len);
bool    ssq_response_is_truncated(const uint8_t *response, size_t response_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !RESPONSE_H */
