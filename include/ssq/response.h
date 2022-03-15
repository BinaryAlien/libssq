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
 * @param response     the response's buffer
 * @param response_len length of the response
 * @return true if the response has a challenge header
 */
bool ssq_response_haschall(const char *response, size_t response_len);

/**
 * Gets the challenge in a challenge response.
 * @param response     the challenge response's buffer
 * @param response_len length of the challenge response
 * @return the challenge number in the challenge response
 */
int32_t ssq_response_getchall(const char *response, size_t response_len);

/**
 * Determines if a response was truncated.
 * @param response     the response's buffer
 * @param response_len length of the response
 * @return true if the response starts with a single packet's header
 */
bool ssq_response_istruncated(const char *response, size_t response_len);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_RESPONSE_H */
