#include "error.h"

#include <errno.h>
#include <string.h>

#ifdef _WIN32
# include <winsock2.h>
#endif /* _WIN32 */

void ssq_error_set(struct ssq_error *const error, const enum ssq_error_code code, const char *const message) {
    error->code = code;
#ifdef _WIN32
    strncpy_s(error->message, SSQ_ERROR_MESSAGE_SIZE, message, SSQ_ERROR_MESSAGE_LENGTH);
#else /* !_WIN32 */
    strncpy(error->message, message, SSQ_ERROR_MESSAGE_LENGTH);
#endif /* _WIN32 */
}

void ssq_error_set_from_errno(struct ssq_error *const error) {
    error->code = SSQE_SYSTEM;
#ifdef _WIN32
    strerror_s(error->message, SSQ_ERROR_MESSAGE_SIZE, errno);
#else /* !_WIN32 */
    strerror_r(errno, error->message, SSQ_ERROR_MESSAGE_SIZE);
#endif /* _WIN32 */
}

#ifdef _WIN32
void ssq_error_set_from_wsa(struct ssq_error *const error) {
    error->code = SSQE_SYSTEM;
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        error->message,
        SSQ_ERROR_MESSAGE_SIZE,
        NULL,
    );
}
#endif /* _WIN32 */
