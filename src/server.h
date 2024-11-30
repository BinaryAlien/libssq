#ifndef SERVER_H
#define SERVER_H

#include "ssq/error.h"
#include "ssq/server/timeout.h"

#ifdef _WIN32
# include <ws2tcpip.h>
#else /* !_WIN32 */
# include <netdb.h>
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ssq_server {
    struct addrinfo *addr_list;
    SSQ_ERROR        last_error;
    SSQ_TIMEOUT      timeout;
} SSQ_SERVER;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SERVER_H */
