/* server.h -- Representation of a Source server. */

#ifndef SSQ_SERVER_H
#define SSQ_SERVER_H

#include <stdbool.h>
#include <stdint.h>

#include "ssq/error.h"
#include "ssq/server/timeout.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ssq_server SSQ_SERVER;

SSQ_SERVER    *ssq_server_new(const char *hostname, uint16_t port);
void           ssq_server_free(SSQ_SERVER *server);

#ifdef _WIN32
void           ssq_server_timeout(SSQ_SERVER *server, SSQ_TIMEOUT_SELECTOR which, DWORD value_in_ms);
#else /* !_WIN32 */
void           ssq_server_timeout(SSQ_SERVER *server, SSQ_TIMEOUT_SELECTOR which, time_t value_in_ms);
#endif /* _WIN32 */

bool           ssq_server_ok(const SSQ_SERVER *server);
SSQ_ERROR_CODE ssq_server_err_code(const SSQ_SERVER *server);
const char    *ssq_server_err_msg(const SSQ_SERVER *server);
void           ssq_server_err_clr(SSQ_SERVER *server);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_SERVER_H */
