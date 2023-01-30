#ifndef SSQ_QUERY_H
#define SSQ_QUERY_H

#include <stddef.h>

#include "ssq/server.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t *ssq_query(SSQ_SERVER *server, const uint8_t *payload, size_t payload_len, size_t *response_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_QUERY_H */
