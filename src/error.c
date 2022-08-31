#include "ssq/error.h"

#include <errno.h>
#include <string.h>

#include "ssq/helper.h"

#ifdef _WIN32
# include <winsock2.h>
#endif /* _WIN32 */

void ssq_error_set(SSQ_ERROR *err, SSQ_ERROR_CODE code, const char message[]) {
    err->code = code;
    ssq_helper_strncpy(err->message, message, SSQ_ERROR_MESSAGE_LEN);
}

void ssq_error_set_from_errno(SSQ_ERROR *err) {
    err->code = SSQE_SYSTEM;
#ifdef _WIN32
    strerror_s(err->message, SSQ_ERROR_MESSAGE_SIZE, errno);
#else  /* !_WIN32 */
    strerror_r(errno, err->message, SSQ_ERROR_MESSAGE_SIZE);
#endif /* _WIN32 */
}

#ifdef _WIN32
void ssq_error_set_from_wsa(SSQ_ERROR *err) {
    err->code = SSQE_SYSTEM;
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        err->message,
        SSQ_ERROR_MESSAGE_SIZE,
        NULL
    );
}
#endif /* _WIN32 */
