#ifndef SSQ_QUERY_H
#define SSQ_QUERY_H

#include "ssq/ssq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sends a query to a Source server.
 *
 * @param querier      Source server querier to use
 * @param payload      query's payload
 * @param payload_len  length of the query's payload
 * @param response_len where to store the length of the query's response
 *
 * @return dynamically-allocated buffer containing the query's response
 */
uint8_t *ssq_query(SSQ_QUERIER *querier, const uint8_t *payload, size_t payload_len, size_t *response_len);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_QUERY_H */
