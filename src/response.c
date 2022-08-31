#include "ssq/response.h"

#include "ssq/buf.h"
#include "ssq/packet.h"

bool ssq_response_has_challenge(const uint8_t response[], size_t response_len) {
    SSQ_BUF buf;
    ssq_buf_init(&buf, response, response_len);
    uint8_t header = ssq_buf_read_uint8(&buf);
    return header == S2A_HEADER_CHALL;
}

int32_t ssq_response_get_challenge(const uint8_t response[], size_t response_len) {
    SSQ_BUF buf;
    ssq_buf_init(&buf, response, response_len);
    ssq_buf_forward(&buf, 1);
    int32_t chall = ssq_buf_read_int32(&buf);
    return chall;
}

bool ssq_response_is_truncated(const uint8_t response[], size_t response_len) {
    SSQ_BUF buf;
    ssq_buf_init(&buf, response, response_len);
    int32_t first_bytes = ssq_buf_read_int32(&buf);
    return first_bytes == SSQ_PACKET_HEADER_SINGLE;
}
