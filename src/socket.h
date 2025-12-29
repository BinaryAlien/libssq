#ifndef SOCKET_H
#define SOCKET_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif /* !_WIN32 */

#include "ssq/error.h"

#ifdef _WIN32
typedef SOCKET socket_type;
#else /* !_WIN32 */
typedef int socket_type;
#endif /* _WIN32 */

socket_type ssq_socket(struct addrinfo *address, struct ssq_error *error);

#endif /* !SOCKET_H */
