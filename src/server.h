#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
# include <ws2tcpip.h>
#else /* !_WIN32 */
# include <netdb.h>
# include <sys/time.h>
#endif /* _WIN32 */

#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ssq_timeout {
#ifdef _WIN32
    DWORD          recv;
    DWORD          send;
#else /* !_WIN32 */
    struct timeval recv;
    struct timeval send;
#endif /* _WIN32 */
} SSQ_TIMEOUT;

typedef struct ssq_server {
    struct addrinfo *addr_list;
    SSQ_ERROR        last_error;
    SSQ_TIMEOUT      timeout;
} SSQ_SERVER;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SERVER_H */
