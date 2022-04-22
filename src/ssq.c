#include <stdlib.h>
#include <string.h>
#include "ssq/ssq.h"
#include "ssq/helper.h"

SSQ_QUERIER *ssq_init(void) {
    SSQ_QUERIER *const querier = malloc(sizeof (*querier));

    if (querier != NULL) {
        querier->addr_list = NULL;
        ssq_errclr(querier);
        ssq_set_timeout(querier, SSQ_TIMEOUT_RECV, SSQ_TIMEOUT_RECV_DEFAULT_VALUE);
        ssq_set_timeout(querier, SSQ_TIMEOUT_SEND, SSQ_TIMEOUT_SEND_DEFAULT_VALUE);
    }

    return querier;
}

void ssq_free(SSQ_QUERIER *const querier) {
    freeaddrinfo(querier->addr_list);
    free(querier);
}

void ssq_set_target(SSQ_QUERIER *const querier, const char hostname[], const uint16_t port) {
    freeaddrinfo(querier->addr_list);

    char port_str[SSQ_PORT_SIZE] = { '\0' };
    ssq_helper_port_to_str(port, port_str);

    struct addrinfo hints;
    memset(&hints, 0, sizeof (hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    const int gai_errnum = getaddrinfo(hostname, port_str, &hints, &(querier->addr_list));
    if (gai_errnum != 0) {
        querier->addr_list = NULL;
        ssq_error_set(&(querier->err), SSQ_ERR_SYS, gai_strerror(gai_errnum));
    }
}

void ssq_set_timeout(
    SSQ_QUERIER *const querier,
    const SSQ_TIMEOUT  which,
#ifdef _WIN32
    const DWORD        value_in_ms
#else /* not _WIN32 */
    const time_t       value_in_ms
#endif /* _WIN32 */
) {
#ifdef _WIN32
    if (which & SSQ_TIMEOUT_RECV)
        querier->timeout_recv = value_in_ms;
    if (which & SSQ_TIMEOUT_SEND)
        querier->timeout_send = value_in_ms;
#else /* not _WIN32 */
    if (which & SSQ_TIMEOUT_RECV)
        ssq_helper_ms_to_tv(value_in_ms, &(querier->timeout_recv));
    if (which & SSQ_TIMEOUT_SEND)
        ssq_helper_ms_to_tv(value_in_ms, &(querier->timeout_send));
#endif /* _WIN32 */
}

SSQ_ERROR_CODE ssq_errc(const SSQ_QUERIER *const querier) {
    return querier->err.code;
}

bool ssq_ok(const SSQ_QUERIER *const querier) {
    return ssq_errc(querier) == SSQ_OK;
}

const char *ssq_errm(const SSQ_QUERIER *const querier) {
    return querier->err.message;
}

void ssq_errclr(SSQ_QUERIER *const querier) {
    ssq_error_clear(&(querier->err));
}
