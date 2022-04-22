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

/**
 * @return minimum `size_t' between `x' and `y'
 */
static inline size_t ssq_helper_minz(const size_t x, const size_t y) { return (x < y) ? x : y; }

/** Portable version of `strncpy'. */
static inline void ssq_helper_strncpy(char dst[], const char src[], size_t len) {
#if _WIN32
    strncpy_s(dst, len + 1, src, len);
#else /* not _WIN32 */
    strncpy(dst, src, len);
#endif /* _WIN32 */
}

/**
 * Converts a port number to its text decimal representation.
 *
 * @param port port number
 * @param out  where to store the output string
 *
 * @return return value of the underlying `snprintf' call
 */
static inline int ssq_helper_port_to_str(const uint16_t port, char out[SSQ_PORT_SIZE]) {
#ifdef _WIN32
    return sprintf_s(out, SSQ_PORT_SIZE, "%" PRIu16, port);
#else /* not _WIN32 */
    return snprintf(out, SSQ_PORT_SIZE, "%" PRIu16, port);
#endif /* _WIN32 */
}

#ifndef _WIN32
/**
 * Fills a `timeval' struct from a timeout value in milliseconds.
 *
 * @param value_in_ms timeout value in milliseconds
 * @param out         `timeval' struct to fill
 */
static inline void ssq_helper_ms_to_tv(const time_t value_in_ms, struct timeval *const out) {
    out->tv_sec  = value_in_ms / 1000;
    out->tv_usec = value_in_ms % 1000 * 1000;
}
#endif /* _WIN32 */

#endif /* SSQ_HELPER_H */
