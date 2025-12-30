#ifndef SERVER_H
#define SERVER_H

#include "ssq/server.h"

#ifdef _WIN32
#include <ws2tcpip.h>
#else /* !_WIN32 */
#include <netdb.h>
#endif /* _WIN32 */

#include "error.h"
#include "timeout.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_server {
  struct addrinfo *addresses;
  struct ssq_error last_error;
  struct ssq_timeout timeout;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SERVER_H */
