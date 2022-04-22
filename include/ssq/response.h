#ifndef SSQ_RESPONSE_H
#define SSQ_RESPONSE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define S2A_HEADER_CHALL 0x41

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Determines if a response has a challenge.
 *
 * @param response     response buffer
 * @param response_len length of the response
 *
 * @return true if the response has a challenge header
 */
bool ssq_response_has_challenge(const uint8_t *response, size_t response_len);

/**
 * Gets the challenge in a challenge response.
 *
 * @param response     challenge response buffer
 * @param response_len length of the challenge response
 *
 * @return challenge number in the challenge response
 */
int32_t ssq_response_get_challenge(const uint8_t *response, size_t response_len);

/**
 * Determines if a response was truncated.
 *
 * @param response     response buffer
 * @param response_len length of the response
 *
 * @return true if the response starts with a single packet response header
 */
bool ssq_response_is_truncated(const uint8_t *response, size_t response_len);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_RESPONSE_H */
