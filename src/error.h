#ifndef ERROR_H
#define ERROR_H

#include "ssq/error.h"

#define SSQ_ERROR_MESSAGE_LENGTH 511
#define SSQ_ERROR_MESSAGE_SIZE (SSQ_ERROR_MESSAGE_LENGTH + 1)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_error {
  enum ssq_error_code code;
  char message[SSQ_ERROR_MESSAGE_SIZE];
};

void ssq_error_set(struct ssq_error *error, enum ssq_error_code code,
                   const char *message);

void ssq_error_set_from_errno(struct ssq_error *error);

#ifdef _WIN32
void ssq_error_set_from_wsa(struct ssq_error *error);
#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !ERROR_H */
