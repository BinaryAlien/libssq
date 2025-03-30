#include "error.h"

#include <errno.h>
#include <string.h>

#include "helper.h"

#ifdef _WIN32
# include <winsock2.h>
#endif /* _WIN32 */

void ssq_error_set(SSQ_ERROR *error, SSQ_ERROR_CODE code, const char message[]) {
    error->code = code;
    ssq_helper_strncpy(error->message, message, SSQ_ERROR_MESSAGE_LEN);
}

void ssq_error_set_from_errno(SSQ_ERROR *error) {
    error->code = SSQE_SYSTEM;
#ifdef _WIN32
    strerror_s(error->message, SSQ_ERROR_MESSAGE_SIZE, errno);
#else /* !_WIN32 */
    strerror_r(errno, error->message, SSQ_ERROR_MESSAGE_SIZE);
#endif /* _WIN32 */
}

#ifdef _WIN32
void ssq_error_set_from_wsa(SSQ_ERROR *error) {
    error->code = SSQE_SYSTEM;
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        error->message,
        SSQ_ERROR_MESSAGE_SIZE,
        NULL
    );
}
#endif /* _WIN32 */
