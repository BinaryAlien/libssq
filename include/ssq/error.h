#ifndef SSQ_ERROR_H
#define SSQ_ERROR_H

#define SSQ_ERRMSG_LEN  511
#define SSQ_ERRMSG_SIZE (SSQ_ERRMSG_LEN + 1)

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
    char           message[SSQ_ERRMSG_SIZE];
} SSQ_ERROR;

/**
 * Sets the error of an `SSQ_ERROR` struct.
 * @param err     the `SSQ_ERROR` struct
 * @param code    error code to set
 * @param message error message to copy
 */
void ssq_error_set(SSQ_ERROR *err, SSQ_ERROR_CODE code, const char *message);

/**
 * Sets the error of an `SSQ_ERROR` struct to the last `errno` error.
 * @param err the `SSQ_ERROR` struct
 */
void ssq_error_set_sys(SSQ_ERROR *err);

#ifdef _WIN32
/**
 * Sets the error of an `SSQ_ERROR` struct to the last WSA error.
 * @param err the `SSQ_ERROR` struct
 */
void ssq_error_set_wsa(SSQ_ERROR *err);
#endif /* _WIN32 */

/**
 * Clears the error of an `SSQ_ERROR` struct.
 * @param err the `SSQ_ERROR` struct to clear
 */
void ssq_error_clear(SSQ_ERROR *err);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_ERROR_H */
