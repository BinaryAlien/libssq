#ifndef STREAM_H
#define STREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_stream {
  uint8_t *begin;
  uint8_t *end;
  uint8_t *cursor;
};

void ssq_stream_initialize(struct ssq_stream *stream, const void *buffer,
                           size_t size);

size_t ssq_stream_available(const struct ssq_stream *stream);
size_t ssq_stream_position(const struct ssq_stream *stream);

size_t ssq_stream_forward(struct ssq_stream *stream, size_t count);

void ssq_stream_read(struct ssq_stream *stream, void *buffer, size_t count);
int8_t ssq_stream_read_i8(struct ssq_stream *stream);
int16_t ssq_stream_read_i16(struct ssq_stream *stream);
int32_t ssq_stream_read_i32(struct ssq_stream *stream);
int64_t ssq_stream_read_i64(struct ssq_stream *stream);
uint8_t ssq_stream_read_u8(struct ssq_stream *stream);
uint16_t ssq_stream_read_u16(struct ssq_stream *stream);
uint32_t ssq_stream_read_u32(struct ssq_stream *stream);
uint64_t ssq_stream_read_u64(struct ssq_stream *stream);
float ssq_stream_read_f32(struct ssq_stream *stream);
double ssq_stream_read_f64(struct ssq_stream *stream);
bool ssq_stream_read_bool(struct ssq_stream *stream);
char *ssq_stream_read_string(struct ssq_stream *stream, size_t *size);

void ssq_stream_write(struct ssq_stream *stream, const void *buffer,
                      size_t count);
void ssq_stream_write_i8(struct ssq_stream *stream, int8_t value);
void ssq_stream_write_i16(struct ssq_stream *stream, int16_t value);
void ssq_stream_write_i32(struct ssq_stream *stream, int32_t value);
void ssq_stream_write_i64(struct ssq_stream *stream, int64_t value);
void ssq_stream_write_u8(struct ssq_stream *stream, uint8_t value);
void ssq_stream_write_u16(struct ssq_stream *stream, uint16_t value);
void ssq_stream_write_u32(struct ssq_stream *stream, uint32_t value);
void ssq_stream_write_u64(struct ssq_stream *stream, uint64_t value);
void ssq_stream_write_f32(struct ssq_stream *stream, float value);
void ssq_stream_write_f64(struct ssq_stream *stream, double value);
void ssq_stream_write_bool(struct ssq_stream *stream, bool value);
void ssq_stream_write_string(struct ssq_stream *stream, const char *value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !STREAM_H */
