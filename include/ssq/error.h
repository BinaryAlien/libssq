#ifndef SSQ_ERROR_H
#define SSQ_ERROR_H

#define SSQ_ERROR_MESSAGE_LEN  511
#define SSQ_ERROR_MESSAGE_SIZE (SSQ_ERROR_MESSAGE_LEN + 1)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum ssq_error_code {
    SSQE_OK               = 0,
    SSQE_SYSTEM           = 1,
    SSQE_INVALID_RESPONSE = 2,
    SSQE_UNSUPPORTED      = 3,
    SSQE_GAI              = 4,
    SSQE_NO_SOCKET        = 5,
} SSQ_ERROR_CODE;

typedef struct ssq_error {
    SSQ_ERROR_CODE code;
    char           message[SSQ_ERROR_MESSAGE_SIZE];
} SSQ_ERROR;

void ssq_error_set(SSQ_ERROR *err, SSQ_ERROR_CODE code, const char *message);
void ssq_error_set_from_errno(SSQ_ERROR *err);
#ifdef _WIN32
void ssq_error_set_from_wsa(SSQ_ERROR *err);
#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_ERROR_H */
