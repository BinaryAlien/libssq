#ifndef QUERY_H
#define QUERY_H

#include <stddef.h>
#include <stdint.h>

#include "response.h"
#include "ssq/server.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_response *ssq_query(struct ssq_server *server,
                               const uint8_t *payload, size_t size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !QUERY_H */
