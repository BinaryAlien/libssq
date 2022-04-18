#include "ssq/buf.h"
#include "ssq/packet.h"
#include "ssq/response.h"

bool ssq_response_haschall(const char response[], const size_t response_len) {
    SSQ_BUF       buf    = ssq_buf_init(response, response_len);
    const uint8_t header = ssq_buf_get_uint8(&buf);
    return header == S2A_HEADER_CHALL;
}

int32_t ssq_response_getchall(const char response[], const size_t response_len) {
    SSQ_BUF buf = ssq_buf_init(response, response_len);

    ssq_buf_forward(&buf, 1); // skip header
    const int32_t chall = ssq_buf_get_int32(&buf);

    return chall;
}

bool ssq_response_istruncated(const char response[], const size_t response_len) {
    SSQ_BUF       buf         = ssq_buf_init(response, response_len);
    const int32_t first_bytes = ssq_buf_get_int32(&buf);
    return first_bytes == A2S_PACKET_HEADER_SINGLE;
}
