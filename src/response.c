#include "ssq/response.h"

#include "ssq/packet.h"
#include "ssq/stream.h"

bool ssq_response_has_challenge(const uint8_t response[], size_t response_len) {
    SSQ_STREAM stream;
    ssq_stream_wrap(&stream, response, response_len);
    uint8_t header = ssq_stream_read_uint8_t(&stream);
    return header == S2A_HEADER_CHALL;
}

int32_t ssq_response_get_challenge(const uint8_t response[], size_t response_len) {
    SSQ_STREAM stream;
    ssq_stream_wrap(&stream, response, response_len);
    ssq_stream_advance(&stream, sizeof (uint8_t));
    return ssq_stream_read_int32_t(&stream);
}

bool ssq_response_is_truncated(const uint8_t response[], size_t response_len) {
    SSQ_STREAM stream;
    ssq_stream_wrap(&stream, response, response_len);
    int32_t first_bytes = ssq_stream_read_int32_t(&stream);
    return first_bytes == SSQ_PACKET_HEADER_SINGLE;
}
