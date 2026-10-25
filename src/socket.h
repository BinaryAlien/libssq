#ifndef SOCKET_H
#define SOCKET_H

#include <stdbool.h>
#include <stddef.h>

#ifdef _WIN32
#include <winsock2.h>
#endif /* _WIN32 */

#include "error.h"
#include "server.h"
#include "timeout.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32
typedef SOCKET ssq_socket;
#else  /* !_WIN32 */
typedef int ssq_socket;
#endif /* _WIN32 */

ssq_socket ssq_socket_open(struct ssq_server *server);

void ssq_socket_close(ssq_socket descriptor);

bool ssq_socket_ok(ssq_socket descriptor);

bool ssq_socket_set_timeout(ssq_socket descriptor,
                            const struct ssq_timeout *timeout,
                            struct ssq_error *error);

size_t ssq_socket_recv(ssq_socket descriptor, void *buffer, size_t count,
                       struct ssq_error *error);

bool ssq_socket_send(ssq_socket descriptor, const void *payload,
                     size_t payload_size, struct ssq_error *error);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SOCKET_H */
