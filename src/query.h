#ifndef QUERY_H
#define QUERY_H

#include <stddef.h>

#include "response.h"
#include "server.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_response *ssq_query(struct ssq_server *server, const void *payload,
                               size_t size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !QUERY_H */
