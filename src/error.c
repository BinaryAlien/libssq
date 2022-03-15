#include <errno.h>
#include <string.h>
#include "ssq/error.h"
#include "ssq/helper.h"

#ifdef _WIN32
# include <Winsock2.h>
#endif /* _WIN32 */

void ssq_error_set(
    SSQ_ERROR     *const err,
    const SSQ_ERROR_CODE code,
    const char           message[]
) {
    err->code = code;
    ssq_helper_strncpy(err->message, message, SSQ_ERRMSG_LEN);
}

void ssq_error_set_sys(SSQ_ERROR *const err) {
    err->code = SSQ_ERR_SYS;

#ifdef _WIN32
    strerror_s(err->message, SSQ_ERRMSG_SIZE, errno);
#else /* not _WIN32 */
    strerror_r(errno, err->message, SSQ_ERRMSG_SIZE);
#endif /* _WIN32 */
}

#ifdef _WIN32
void ssq_error_set_wsa(SSQ_ERROR *const err) {
    err->code = SSQ_ERR_SYS;

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        err->message,
        SSQ_ERRMSG_SIZE,
        NULL
    );
}
#endif /* _WIN32 */

void ssq_error_clear(SSQ_ERROR *const err) {
    err->code       = SSQ_OK;
    err->message[0] = '\0';
}
