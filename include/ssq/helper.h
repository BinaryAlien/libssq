/* helper.h -- Utility functions. */

#ifndef SSQ_HELPER_H
#define SSQ_HELPER_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
# include <winsock2.h>
#else /* !_WIN32 */
# include <sys/time.h>
#endif /* _WIN32 */

#define SSQ_PORT_LEN  5
#define SSQ_PORT_SIZE (SSQ_PORT_LEN + 1)

static inline size_t ssq_helper_minz(size_t x, size_t y) {
    return (x < y) ? x : y;
}

static inline void ssq_helper_strncpy(char dest[], const char src[], size_t len) {
#ifdef _WIN32
    strncpy_s(dest, len + 1, src, len);
#else /* !_WIN32 */
    strncpy(dest, src, len);
#endif /* _WIN32 */
}

static inline int ssq_helper_port_to_str(uint16_t port, char port_str[SSQ_PORT_SIZE]) {
#ifdef _WIN32
    return sprintf_s(port_str, SSQ_PORT_SIZE, "%" PRIu16, port);
#else /* !_WIN32 */
    return snprintf(port_str, SSQ_PORT_SIZE, "%" PRIu16, port);
#endif /* _WIN32 */
}

#ifndef _WIN32
static inline void ssq_helper_millis_to_timeval(time_t value_in_ms, struct timeval *tv) {
    tv->tv_sec = value_in_ms / 1000;
    tv->tv_usec = value_in_ms % 1000 * 1000;
}
#endif /* !_WIN32 */

#endif /* !SSQ_HELPER_H */
