/* timeout.h -- Query timeouts. */

#ifndef SSQ_SERVER_TIMEOUT_H
#define SSQ_SERVER_TIMEOUT_H

#ifdef _WIN32
# include <ws2tcpip.h>
#else /* !_WIN32 */
# include <sys/time.h>
#endif /* _WIN32 */

#ifndef SSQ_TIMEOUT_RECV_DEFAULT
# define SSQ_TIMEOUT_RECV_DEFAULT 5000 // ms
#endif /* !SSQ_TIMEOUT_RECV_DEFAULT */
#ifndef SSQ_TIMEOUT_SEND_DEFAULT
# define SSQ_TIMEOUT_SEND_DEFAULT 5000 // ms
#endif /* !SSQ_TIMEOUT_SEND_DEFAULT */

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

typedef enum ssq_timeout_selector {
    SSQ_TIMEOUT_RECV = (1 << 0),
    SSQ_TIMEOUT_SEND = (1 << 1),
} SSQ_TIMEOUT_SELECTOR;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_SERVER_TIMEOUT_H */
