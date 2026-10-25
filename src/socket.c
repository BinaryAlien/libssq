#include "socket.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else /* !_WIN32 */
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif /* _WIN32 */

#include "error.h"
#include "server.h"

ssq_socket ssq_socket_open(struct ssq_server *const server) {
  ssq_socket descriptor = INVALID_SOCKET;
  for (const struct addrinfo *address = server->addresses; address != NULL;
       address = address->ai_next) {
    descriptor =
        socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (descriptor == INVALID_SOCKET)
      continue;
    if (connect(descriptor, address->ai_addr, address->ai_addrlen) !=
        SOCKET_ERROR)
      break;
    ssq_socket_close(descriptor);
    descriptor = INVALID_SOCKET;
  }
  if (descriptor == INVALID_SOCKET)
    ssq_error_set(&server->last_error, SSQE_MUTE,
                  "Could not open an endpoint for communication");
  return descriptor;
}

void ssq_socket_close(const ssq_socket descriptor) {
#ifdef _WIN32
  closesocket(descriptor);
#else  /* !_WIN32 */
  close(descriptor);
#endif /* _WIN32 */
}

bool ssq_socket_ok(const ssq_socket descriptor) {
  return descriptor != INVALID_SOCKET;
}

bool ssq_socket_set_timeout(const ssq_socket descriptor,
                            const struct ssq_timeout *const timeout,
                            struct ssq_error *const error) {
  if (setsockopt(descriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout->recv,
                 sizeof(timeout->recv)) == SOCKET_ERROR) {
    ssq_error_set_from_errno(error);
    return false;
  }
  if (setsockopt(descriptor, SOL_SOCKET, SO_SNDTIMEO, &timeout->send,
                 sizeof(timeout->send)) == SOCKET_ERROR) {
    ssq_error_set_from_errno(error);
    return false;
  }
  return true;
}

size_t ssq_socket_recv(const ssq_socket descriptor, void *const buffer,
                       const size_t count, struct ssq_error *const error) {
#ifdef _WIN32
  const int recv_count = recv(descriptor, (char *)buffer, count, 0);
#else  /* !_WIN32 */
  const ssize_t recv_count = recv(descriptor, buffer, count, 0);
#endif /* _WIN32 */
  if (recv_count == SOCKET_ERROR) {
#ifdef _WIN32
    ssq_error_set_from_wsa(error);
#else  /* !_WIN32 */
    ssq_error_set_from_errno(error);
#endif /* _WIN32 */
  }
  return (size_t)recv_count;
}

bool ssq_socket_send(const ssq_socket descriptor, const void *const payload,
                     const size_t payload_size, struct ssq_error *error) {
  if (send(descriptor, payload, payload_size, 0) == SOCKET_ERROR) {
#ifdef _WIN32
    ssq_error_set_from_wsa(error);
#else  /* !_WIN32 */
    ssq_error_set_from_errno(error);
#endif /* _WIN32 */
    return false;
  }
  return true;
}
