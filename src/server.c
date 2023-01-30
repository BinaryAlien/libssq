#include "ssq/server.h"

#include <string.h>

#include "ssq/helper.h"

static void prepare_udp_hints(struct addrinfo *hints) {
    memset(hints, 0, sizeof (*hints));
    hints->ai_family   = AF_INET;
    hints->ai_socktype = SOCK_DGRAM;
}

static int resolve_address(struct addrinfo **dest, const char hostname[], uint16_t port) {
    char port_str[SSQ_PORT_SIZE] = { '\0' };
    ssq_helper_port_to_str(port, port_str);
    struct addrinfo hints;
    prepare_udp_hints(&hints);
    return getaddrinfo(hostname, port_str, &hints, dest);
}

bool ssq_server_init(SSQ_SERVER *server, const char hostname[], uint16_t port) {
    int gai_ecode = resolve_address(&server->addr_list, hostname, port);
    if (gai_ecode == 0) {
        ssq_server_err_clr(server);
        ssq_server_timeout(server, SSQ_TIMEOUT_RECV, SSQ_SERVER_DEFAULT_TIMEOUT_RECV);
        ssq_server_timeout(server, SSQ_TIMEOUT_SEND, SSQ_SERVER_DEFAULT_TIMEOUT_SEND);
    } else {
        ssq_error_set(&server->last_error, SSQE_GAI, gai_strerror(gai_ecode));
    }
    return ssq_server_ok(server);
}

void ssq_server_cleanup(SSQ_SERVER *server) {
    freeaddrinfo(server->addr_list);
}

#ifdef _WIN32
void ssq_server_timeout(SSQ_SERVER *server, SSQ_TIMEOUT_SELECTOR which, DWORD value_in_ms) {
    if (which & SSQ_TIMEOUT_RECV)
        server->timeout.recv = value_in_ms;
    if (which & SSQ_TIMEOUT_SEND)
        server->timeout.send = value_in_ms;
}
#else /* !_WIN32 */
void ssq_server_timeout(SSQ_SERVER *server, SSQ_TIMEOUT_SELECTOR which, time_t value_in_ms) {
    if (which & SSQ_TIMEOUT_RECV)
        ssq_helper_fill_timeval(value_in_ms, &server->timeout.recv);
    if (which & SSQ_TIMEOUT_SEND)
        ssq_helper_fill_timeval(value_in_ms, &server->timeout.send);
}
#endif /* _WIN32 */

bool           ssq_server_ok(const SSQ_SERVER *server) { return ssq_server_err_code(server) == SSQE_OK; }
SSQ_ERROR_CODE ssq_server_err_code(const SSQ_SERVER *server) { return server->last_error.code; }
const char    *ssq_server_err_msg(const SSQ_SERVER *server) { return server->last_error.message; }

void ssq_server_err_clr(SSQ_SERVER *server) {
    server->last_error.code = SSQE_OK;
    server->last_error.message[0] = '\0';
}
