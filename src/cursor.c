#include "cursor.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

void ssq_cursor_initialize(struct ssq_cursor *const cursor,
                           const void *const buffer, const size_t size) {
  cursor->begin = (uint8_t *)buffer;
  cursor->position = cursor->begin;
  cursor->end = cursor->begin + size;
}

size_t ssq_cursor_available(const struct ssq_cursor *const cursor) {
  return cursor->end - cursor->position;
}

size_t ssq_cursor_forward(struct ssq_cursor *const cursor, const size_t count) {
  cursor->position += count;
}

size_t ssq_cursor_position(const struct ssq_cursor *const cursor) {
  return cursor->position - cursor->begin;
}

static size_t minzu(const size_t lhs, const size_t rhs) {
  return lhs < rhs ? lhs : rhs;
}

void ssq_cursor_read(struct ssq_cursor *const cursor, void *const buffer,
                     size_t count) {
  count = minzu(count, ssq_cursor_available(cursor));
  memcpy(buffer, cursor->position, count);
  ssq_cursor_forward(cursor, count);
}

#define SSQ_CURSOR_READ(Type, Shorthand)                                       \
  Type ssq_cursor_read_##Shorthand(struct ssq_cursor *const cursor) {          \
    Type value = 0;                                                            \
    ssq_cursor_read(cursor, &value, sizeof(value));                            \
    return value;                                                              \
  }

SSQ_CURSOR_READ(float, f32);
SSQ_CURSOR_READ(double, f64);

SSQ_CURSOR_READ(int8_t, i8);
SSQ_CURSOR_READ(int16_t, i16);
SSQ_CURSOR_READ(int32_t, i32);
SSQ_CURSOR_READ(int64_t, i64);

SSQ_CURSOR_READ(uint8_t, u8);
SSQ_CURSOR_READ(uint16_t, u16);
SSQ_CURSOR_READ(uint32_t, u32);
SSQ_CURSOR_READ(uint64_t, u64);

bool ssq_cursor_read_bool(struct ssq_cursor *const cursor) {
  return ssq_cursor_read_u8(cursor) != 0;
}

char *ssq_cursor_read_string(struct ssq_cursor *const cursor,
                             size_t *const size) {
  *size = strnlen(cursor->position, ssq_cursor_available(cursor));
  char *const string = malloc(*size + 1);
  if (string == NULL)
    return NULL;
  memcpy(string, cursor->position, *size);
  ssq_cursor_forward(cursor, *size + 1);
  string[*size] = '\0';
  return string;
}

void ssq_cursor_write(struct ssq_cursor *const cursor, const void *const data,
                      const size_t count) {
  memcpy(cursor->position, data, count);
  ssq_cursor_forward(cursor, count);
}

#define SSQ_CURSOR_WRITE(Type, Shorthand)                                      \
  void ssq_cursor_write_##Shorthand(struct ssq_cursor *const cursor,           \
                                    const Type value) {                        \
    ssq_cursor_write(cursor, &value, sizeof(value));                           \
  }

SSQ_CURSOR_WRITE(float, f32);
SSQ_CURSOR_WRITE(double, f64);

SSQ_CURSOR_WRITE(int8_t, i8);
SSQ_CURSOR_WRITE(int16_t, i16);
SSQ_CURSOR_WRITE(int32_t, i32);
SSQ_CURSOR_WRITE(int64_t, i64);

SSQ_CURSOR_WRITE(uint8_t, u8);
SSQ_CURSOR_WRITE(uint16_t, u16);
SSQ_CURSOR_WRITE(uint32_t, u32);
SSQ_CURSOR_WRITE(uint64_t, u64);

void ssq_cursor_write_bool(struct ssq_cursor *const cursor, const bool value) {
  return ssq_cursor_write_u8(cursor, value ? 1 : 0);
}

void ssq_cursor_write_string(struct ssq_cursor *const cursor,
                             const char *const value) {
  const size_t size = strlen(value);
  memcpy(cursor->position, value, size);
  ssq_cursor_forward(cursor, size);
  ssq_cursor_write_u8(cursor, '\0');
}
