#ifndef SSQ_SERVER_H
#define SSQ_SERVER_H

#include <stdbool.h>
#include <stdint.h>
#ifdef _WIN32
# include <ws2tcpip.h>
#else /* !_WIN32 */
# include <netdb.h>
# include <sys/time.h>
#endif /* _WIN32 */
#include "ssq/error.h"

#ifndef SSQ_SERVER_DEFAULT_TIMEOUT_RECV
# define SSQ_SERVER_DEFAULT_TIMEOUT_RECV 5000 // ms
#endif /* !SSQ_SERVER_DEFAULT_TIMEOUT_RECV */
#ifndef SSQ_SERVER_DEFAULT_TIMEOUT_SEND
# define SSQ_SERVER_DEFAULT_TIMEOUT_SEND 5000 // ms
#endif /* !SSQ_SERVER_DEFAULT_TIMEOUT_SEND */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ssq_timeout {
#ifdef _WIN32
    DWORD            recv;
    DWORD            send;
#else /* !_WIN32 */
    struct timeval   recv;
    struct timeval   send;
#endif /* _WIN32 */
} SSQ_TIMEOUT;

typedef struct ssq_server {
    struct addrinfo *addr_list;
    SSQ_ERROR        last_error;
    SSQ_TIMEOUT      timeout;
} SSQ_SERVER;

typedef enum ssq_timeout_selector {
    SSQ_TIMEOUT_RECV = 0x1,
    SSQ_TIMEOUT_SEND = 0x2
} SSQ_TIMEOUT_SELECTOR;

bool           ssq_server_init(SSQ_SERVER *server, const char *hostname, uint16_t port);
void           ssq_server_cleanup(SSQ_SERVER *server);

#ifdef _WIN32
void           ssq_server_timeout(SSQ_SERVER *server, SSQ_TIMEOUT_SELECTOR which, DWORD value_in_ms);
#else /* !_WIN32 */
void           ssq_server_timeout(SSQ_SERVER *server, SSQ_TIMEOUT_SELECTOR which, time_t value_in_ms);
#endif /* _WIN32 */

bool           ssq_server_ok(const SSQ_SERVER *server);
SSQ_ERROR_CODE ssq_server_err_code(const SSQ_SERVER *server);
const char    *ssq_server_err_msg(const SSQ_SERVER *server);
void           ssq_server_err_clr(SSQ_SERVER *server);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_SERVER_H */
