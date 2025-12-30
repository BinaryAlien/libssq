#include "stream.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void ssq_stream_initialize(struct ssq_stream *const stream,
                           const void *const buffer, const size_t size) {
  stream->begin = (uint8_t *)buffer;
  stream->end = stream->begin + size;
  stream->cursor = stream->begin;
}

size_t ssq_stream_available(const struct ssq_stream *const stream) {
  assert(stream->begin <= stream->cursor && stream->cursor <= stream->end);
  return stream->end - stream->cursor;
}

size_t ssq_stream_position(const struct ssq_stream *const stream) {
  assert(stream->begin <= stream->cursor && stream->cursor <= stream->end);
  return stream->cursor - stream->begin;
}

size_t ssq_stream_forward(struct ssq_stream *const stream, const size_t count) {
  stream->cursor += count;
  assert(stream->begin <= stream->cursor && stream->cursor <= stream->end);
}

static size_t minzu(const size_t lhs, const size_t rhs) {
  return lhs < rhs ? lhs : rhs;
}

void ssq_stream_read(struct ssq_stream *const stream, void *const buffer,
                     size_t count) {
  count = minzu(count, ssq_stream_available(stream));
  memcpy(buffer, stream->cursor, count);
  ssq_stream_forward(stream, count);
}

#define SSQ_STREAM_READ(Type, Shorthand)                                       \
  Type ssq_stream_read_##Shorthand(struct ssq_stream *const stream) {          \
    Type value = 0;                                                            \
    ssq_stream_read(stream, &value, sizeof(value));                            \
    return value;                                                              \
  }

SSQ_STREAM_READ(float, f32)
SSQ_STREAM_READ(double, f64)
SSQ_STREAM_READ(int8_t, i8)
SSQ_STREAM_READ(int16_t, i16)
SSQ_STREAM_READ(int32_t, i32)
SSQ_STREAM_READ(int64_t, i64)
SSQ_STREAM_READ(uint8_t, u8)
SSQ_STREAM_READ(uint16_t, u16)
SSQ_STREAM_READ(uint32_t, u32)
SSQ_STREAM_READ(uint64_t, u64)

bool ssq_stream_read_bool(struct ssq_stream *const stream) {
  return ssq_stream_read_u8(stream) != 0;
}

char *ssq_stream_read_string(struct ssq_stream *const stream,
                             size_t *const size) {
  *size = strnlen(stream->cursor, ssq_stream_available(stream));
  char *const string = malloc(*size + 1);
  if (string == NULL)
    return NULL;
  memcpy(string, stream->cursor, *size);
  ssq_stream_forward(stream, *size + 1);
  string[*size] = '\0';
  return string;
}

void ssq_stream_write(struct ssq_stream *const stream, const void *const buffer,
                      const size_t count) {
  memcpy(stream->cursor, buffer, count);
  ssq_stream_forward(stream, count);
}

#define SSQ_STREAM_WRITE(Type, Shorthand)                                      \
  void ssq_stream_write_##Shorthand(struct ssq_stream *const stream,           \
                                    const Type value) {                        \
    ssq_stream_write(stream, &value, sizeof(value));                           \
  }

SSQ_STREAM_WRITE(float, f32)
SSQ_STREAM_WRITE(double, f64)
SSQ_STREAM_WRITE(int8_t, i8)
SSQ_STREAM_WRITE(int16_t, i16)
SSQ_STREAM_WRITE(int32_t, i32)
SSQ_STREAM_WRITE(int64_t, i64)
SSQ_STREAM_WRITE(uint8_t, u8)
SSQ_STREAM_WRITE(uint16_t, u16)
SSQ_STREAM_WRITE(uint32_t, u32)
SSQ_STREAM_WRITE(uint64_t, u64)

void ssq_stream_write_bool(struct ssq_stream *const stream, const bool value) {
  return ssq_stream_write_u8(stream, value ? 1 : 0);
}

void ssq_stream_write_string(struct ssq_stream *const stream,
                             const char *const value) {
  const size_t size = strlen(value);
  memcpy(stream->cursor, value, size);
  ssq_stream_forward(stream, size);
  ssq_stream_write_u8(stream, '\0');
}
