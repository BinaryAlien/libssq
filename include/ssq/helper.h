#ifndef SSQ_HELPER_H
#define SSQ_HELPER_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#ifndef _WIN32
# include <sys/time.h>
#endif /* _WIN32 */

#define SSQ_PORT_LEN  5
#define SSQ_PORT_SIZE (SSQ_PORT_LEN + 1)

static inline size_t minz(const size_t x, const size_t y) {
    return (x < y) ? x : y;
}

/**
 * Portable version of `strncpy`.
 */
static inline void ssq_helper_strncpy(char dst[], const char src[], size_t len) {
#if _WIN32
    strncpy_s(dst, len + 1, src, len);
#else /* not _WIN32 */
    strncpy(dst, src, len);
#endif /* _WIN32 */
}

/**
 * Converts a numeric port number to a string.
 * @param port the numeric port number
 * @param out  where to store the output string
 */
static inline void ssq_helper_port2str(const uint16_t port, char out[SSQ_PORT_SIZE]) {
#ifdef _WIN32
    sprintf_s(out, SSQ_PORT_SIZE, "%" PRIu16, port);
#else /* not _WIN32 */
    snprintf(out, SSQ_PORT_SIZE, "%" PRIu16, port);
#endif /* _WIN32 */
}

#ifndef _WIN32
/**
 * Converts a timeout value in milliseconds to a `timeval` struct.
 * @param value_in_ms the timeout value in milliseconds
 * @param out         the output `timeval` struct
 */
static inline void ssq_helper_ms2tv(const time_t value_in_ms, struct timeval *const out) {
    out->tv_sec  = value_in_ms / 1000;
    out->tv_usec = value_in_ms % 1000 * 1000;
}
#endif /* _WIN32 */

#endif /* SSQ_HELPER_H */
