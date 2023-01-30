#include "ssq/stream.h"

#include <stdlib.h>
#include <string.h>

#include "ssq/helper.h"

void ssq_stream_wrap(SSQ_STREAM *stream, const void *buf, size_t size) {
    stream->buf = buf;
    stream->size = size;
    stream->pos = 0;
}

void ssq_stream_advance(SSQ_STREAM *stream, size_t n) {
    stream->pos += n;
}

size_t ssq_stream_remaining(const SSQ_STREAM *stream) {
    return ssq_stream_end(stream) ? 0 : (stream->size - stream->pos);
}

bool ssq_stream_end(const SSQ_STREAM *stream) {
    return stream->pos >= stream->size;
}

void ssq_stream_read(SSQ_STREAM *src, void *dest, size_t n) {
    if (n <= ssq_stream_remaining(src)) {
        memcpy(dest, src->buf + src->pos, n);
        ssq_stream_advance(src, n);
    } else {
        memset(dest, 0, n);
    }
}

#define SSQ_STREAM_READ_DECL(Type)                       \
    Type ssq_stream_read_##Type(SSQ_STREAM *stream) {    \
        Type value;                                      \
        ssq_stream_read(stream, &value, sizeof (value)); \
        return value;                                    \
    }

SSQ_STREAM_READ_DECL(int8_t)
SSQ_STREAM_READ_DECL(int16_t)
SSQ_STREAM_READ_DECL(int32_t)
SSQ_STREAM_READ_DECL(int64_t)
SSQ_STREAM_READ_DECL(uint8_t)
SSQ_STREAM_READ_DECL(uint16_t)
SSQ_STREAM_READ_DECL(uint32_t)
SSQ_STREAM_READ_DECL(uint64_t)
SSQ_STREAM_READ_DECL(float)
SSQ_STREAM_READ_DECL(double)

bool ssq_stream_read_bool(SSQ_STREAM *stream) {
    return ssq_stream_read_uint8_t(stream);
}

/* Compute the number of bytes until the next null byte or end of stream. */
static size_t ssq_stream_read_string_len(const SSQ_STREAM *stream) {
    size_t len = 0;
    while (len < ssq_stream_remaining(stream) && stream->buf[stream->pos + len] != '\0')
        ++len;
    return len;
}

char *ssq_stream_read_string(SSQ_STREAM *stream, size_t *len) {
    *len = ssq_stream_read_string_len(stream);
    char *dest = calloc(*len + 1, sizeof (*dest));
    if (dest == NULL)
        return NULL;
    const char *src = (const char *)(stream->buf + stream->pos);
    ssq_helper_strncpy(dest, src, *len);
    ssq_stream_advance(stream, *len + 1);
    return dest;
}
