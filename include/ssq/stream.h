/* stream.h -- Data input stream. */

#ifndef SSQ_STREAM_H
#define SSQ_STREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ssq_stream {
    const uint8_t *data;
    size_t         size;
    size_t         pos;
} SSQ_STREAM;

void     ssq_stream_wrap(SSQ_STREAM *stream, const void *data, size_t size);

void     ssq_stream_advance(SSQ_STREAM *stream, size_t n);
size_t   ssq_stream_remaining(const SSQ_STREAM *stream);
bool     ssq_stream_end(const SSQ_STREAM *stream);

void     ssq_stream_read(SSQ_STREAM *src, void *dest, size_t n);
int8_t   ssq_stream_read_int8_t(SSQ_STREAM *stream);
int16_t  ssq_stream_read_int16_t(SSQ_STREAM *stream);
int32_t  ssq_stream_read_int32_t(SSQ_STREAM *stream);
int64_t  ssq_stream_read_int64_t(SSQ_STREAM *stream);
uint8_t  ssq_stream_read_uint8_t(SSQ_STREAM *stream);
uint16_t ssq_stream_read_uint16_t(SSQ_STREAM *stream);
uint32_t ssq_stream_read_uint32_t(SSQ_STREAM *stream);
uint64_t ssq_stream_read_uint64_t(SSQ_STREAM *stream);
float    ssq_stream_read_float(SSQ_STREAM *stream);
double   ssq_stream_read_double(SSQ_STREAM *stream);
bool     ssq_stream_read_bool(SSQ_STREAM *stream);
char    *ssq_stream_read_string(SSQ_STREAM *stream, size_t *len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_STREAM_H */
