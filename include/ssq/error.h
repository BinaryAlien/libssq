#ifndef SSQ_ERROR_H
#define SSQ_ERROR_H

#define SSQ_ERROR_MESSAGE_LEN  511
#define SSQ_ERROR_MESSAGE_SIZE (SSQ_ERROR_MESSAGE_LEN + 1)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ssq_error_code {
    SSQ_OK,              /* no error              */
    SSQ_ERR_SYS,         /* system error          */
    SSQ_ERR_BADRES,      /* bad response          */
    SSQ_ERR_UNSUPPORTED, /* unsupported feature   */
    SSQ_ERR_NOENDPOINT,  /* no endpoint available */
} SSQ_ERROR_CODE;

typedef struct ssq_error {
    SSQ_ERROR_CODE code;
    char           message[SSQ_ERROR_MESSAGE_SIZE];
} SSQ_ERROR;

/**
 * Sets the error of an `SSQ_ERROR' struct.
 *
 * @param err     `SSQ_ERROR' struct
 * @param code    code to set
 * @param message message to set
 */
void ssq_error_set(SSQ_ERROR *err, SSQ_ERROR_CODE code, const char *message);

/**
 * Sets the code of an `SSQ_ERROR' struct to system error and
 * sets the message to the corresponding `errno' error.
 *
 * @param err `SSQ_ERROR' struct
 */
void ssq_error_set_from_errno(SSQ_ERROR *err);

#ifdef _WIN32
/**
 * Sets the code of an `SSQ_ERROR' struct to system error
 * and sets the message to the corresponding WSA error.
 *
 * @param err `SSQ_ERROR' struct
 */
void ssq_error_set_from_wsa(SSQ_ERROR *err);
#endif /* _WIN32 */

/**
 * Clears the error of an `SSQ_ERROR' struct.
 * @param err `SSQ_ERROR' struct to clear
 */
void ssq_error_clear(SSQ_ERROR *err);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_ERROR_H */
