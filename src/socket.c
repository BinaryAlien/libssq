#include "socket.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else /* !_WIN32 */
# include <sys/socket.h>
# include <unistd.h>
# define INVALID_SOCKET (-1)
# define SOCKET_ERROR   (-1)
# define closesocket    close
#endif /* _WIN32 */

#include "ssq/error.h"

socket_type ssq_socket_connect(
    struct addrinfo  *const address,
    struct ssq_error *const error
) {
    const socket_type sock = socket(
        address->ai_family,
        address->ai_socktype,
        address->ai_protocol
    );
    if (sock == INVALID_SOCKET) {
#ifdef _WIN32
        ssq_error_set_from_wsa(error);
#else /* !_WIN32 */
        ssq_error_set_from_errno(error);
#endif /* _WIN32 */
        return INVALID_SOCKET;
    }
    if (connect(sock, address->ai_addr, address->ai_addrlen) == SOCKET_ERROR) {
        closesocket(sock);
        return INVALID_SOCKET;
    }
    return sock;
}
