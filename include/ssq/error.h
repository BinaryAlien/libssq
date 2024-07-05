/* error.h -- Error information. */

#ifndef SSQ_ERROR_H
#define SSQ_ERROR_H

#define SSQ_ERROR_MESSAGE_LEN  511
#define SSQ_ERROR_MESSAGE_SIZE (SSQ_ERROR_MESSAGE_LEN + 1)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum ssq_error_code {
    SSQE_OK = 0,
    SSQE_SYSTEM,
    SSQE_INVALID_RESPONSE,
    SSQE_UNSUPPORTED,
    SSQE_GAI,
    SSQE_NO_SOCKET,
} SSQ_ERROR_CODE;

typedef struct ssq_error {
    SSQ_ERROR_CODE code;
    char           message[SSQ_ERROR_MESSAGE_SIZE];
} SSQ_ERROR;

void ssq_error_set(SSQ_ERROR *error, SSQ_ERROR_CODE code, const char *message);
void ssq_error_clear(SSQ_ERROR *error);
void ssq_error_set_from_errno(SSQ_ERROR *error);
#ifdef _WIN32
void ssq_error_set_from_wsa(SSQ_ERROR *error);
#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_ERROR_H */
