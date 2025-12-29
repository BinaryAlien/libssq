#ifndef SSQ_SERVER_H
#define SSQ_SERVER_H

#include <stdbool.h>
#include <stdint.h>

#include "ssq/error.h"
#include "ssq/timeout.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_server;

struct ssq_server  *ssq_server_from_address(const char *host, uint16_t port);
void                ssq_server_free(struct ssq_server *server);

void                ssq_server_timeout(struct ssq_server *server, enum ssq_timeout_selector which, ssq_milliseconds duration);

void                ssq_server_eclear(struct ssq_server *server);
enum ssq_error_code ssq_server_ecode(const struct ssq_server *server);
const char         *ssq_server_emessage(const struct ssq_server *server);
bool                ssq_server_eok(const struct ssq_server *server);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_SERVER_H */
