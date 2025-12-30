#include "server.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else /* !_WIN32 */
#include <netdb.h>
#include <sys/socket.h>
#endif /* _WIN32 */

static struct ssq_server *ssq_server_alloc(void) {
  struct ssq_server *const server = malloc(sizeof(*server));
  if (server == NULL)
    return NULL;
  server->addresses = NULL;
  ssq_server_eclear(server);
  ssq_server_timeout(server, SSQ_TIMEOUT_RECV | SSQ_TIMEOUT_SEND, 0);
  return server;
}

static int ssq_server_getaddrinfo(struct ssq_server *const server,
                                  const char *const node,
                                  const char *const service) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  return getaddrinfo(node, service, &hints, &server->addresses);
}

static int ssq_server_resolve(struct ssq_server *const server,
                              const char *const host, const uint16_t port) {
  char service[6] = "";
#ifdef _WIN32
  sprintf_s(service, sizeof(service), "%" PRIu16, port);
#else  /* !_WIN32 */
  snprintf(service, sizeof(service), "%" PRIu16, port);
#endif /* _WIN32 */
  return ssq_server_getaddrinfo(server, host, service);
}

struct ssq_server *ssq_server_from_address(const char *const host,
                                           const uint16_t port) {
  struct ssq_server *const server = ssq_server_alloc();
  if (server == NULL)
    return NULL;
  const int gai_errno = ssq_server_resolve(server, host, port);
  if (gai_errno != 0)
    ssq_error_set(&server->last_error, SSQE_GAI, gai_strerror(gai_errno));
  return server;
}

void ssq_server_free(struct ssq_server *const server) {
  if (server == NULL)
    return;
  freeaddrinfo(server->addresses);
  free(server);
}

void ssq_server_timeout(struct ssq_server *const server,
                        const enum ssq_timeout_selector which,
                        const ssq_timeout_milliseconds duration) {
  ssq_timeout_set(&server->timeout, which, duration);
}

void ssq_server_eclear(struct ssq_server *const server) {
  server->last_error.code = SSQE_OK;
  server->last_error.message[0] = '\0';
}

enum ssq_error_code ssq_server_ecode(const struct ssq_server *const server) {
  return server->last_error.code;
}

const char *ssq_server_emessage(const struct ssq_server *const server) {
  return server->last_error.message;
}

bool ssq_server_eok(const struct ssq_server *const server) {
  return ssq_server_ecode(server) == SSQE_OK;
}
