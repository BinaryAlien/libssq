#ifndef SSQ_SSQ_H
#define SSQ_SSQ_H

#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
# include <ws2tcpip.h>
#else /* not _WIN32 */
# include <netdb.h>
# include <sys/time.h>
#endif /* _WIN32 */

#include "ssq/error.h"

#define SSQ_TIMEOUT_RECV_DEFAULT_VALUE 5000 // ms
#define SSQ_TIMEOUT_SEND_DEFAULT_VALUE 5000 // ms

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ssq_timeout {
    SSQ_TIMEOUT_RECV = 0x1,
    SSQ_TIMEOUT_SEND = 0x2
} SSQ_TIMEOUT;

typedef struct ssq_querier {
    struct addrinfo *addr_list;
    struct ssq_error err;

#ifdef _WIN32
    DWORD            timeout_recv;
    DWORD            timeout_send;
#else /* not _WIN32 */
    struct timeval   timeout_recv;
    struct timeval   timeout_send;
#endif /* _WIN32 */
} SSQ_QUERIER;

/**
 * Initializes a new Source server querier.
 * @return new dynamically-allocated Source server querier or NULL in case of a memory allocation failure
 */
SSQ_QUERIER *ssq_init(void);

/**
 * Frees a Source server querier.
 * @param querier Source server querier to free
 */
void ssq_free(SSQ_QUERIER *querier);

/**
 * Sets the target server of a Source server querier.
 *
 * @param querier  Source server querier
 * @param hostname target hostname
 * @param port     target port number
 */
void ssq_set_target(SSQ_QUERIER *querier, const char *hostname, uint16_t port);

/**
 * Sets the timeouts of a Source server querier.
 *
 * @param querier     Source server querier
 * @param which       timeouts to set (bitwise)
 * @param value_in_ms value to set in milliseconds
 */
void ssq_set_timeout(
    SSQ_QUERIER *querier,
    SSQ_TIMEOUT  which,
#ifdef _WIN32
    DWORD        value_in_ms
#else /* not _WIN32 */
    time_t       value_in_ms
#endif /* _WIN32 */
);

/**
 * Gets the last error code of a Source server querier.
 * @param querier Source server querier
 * @return last error code of the Source server querier
 */
SSQ_ERROR_CODE ssq_errc(const SSQ_QUERIER *querier);

/**
 * Gets the last error message of a Source server querier.
 * @param querier Source server querier
 * @return last error message of the Source server querier
 */
const char *ssq_errm(const SSQ_QUERIER *querier);

/**
 * Checks if a Source server querier has an error currently set.
 * @param querier Source server querier
 * @return true if the Source server querier has an error currently set
 */
bool ssq_ok(const SSQ_QUERIER *querier);

/**
 * Clears a Source server querier's last error.
 * @param querier Source server querier
 */
void ssq_errclr(SSQ_QUERIER *querier);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_SSQ_H */
