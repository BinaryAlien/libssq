#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "ssq/timeout.h"

#ifdef _WIN32
#include <windows.h>
#else /* !_WIN32 */
#include <sys/time.h>
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_timeout {
#ifdef _WIN32
  DWORD recv;
  DWORD send;
#else  /* !_WIN32 */
  struct timeval recv;
  struct timeval send;
#endif /* _WIN32 */
};

void ssq_timeout_set(struct ssq_timeout *timeout,
                     enum ssq_timeout_selector which,
                     ssq_timeout_milliseconds duration);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !TIMEOUT_H */
