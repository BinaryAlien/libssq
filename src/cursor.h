#ifndef CURSOR_H
#define CURSOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_cursor {
  uint8_t *begin;
  uint8_t *position;
  uint8_t *end;
};

void ssq_cursor_initialize(struct ssq_cursor *cursor, const void *buffer,
                           size_t size);

size_t ssq_cursor_available(const struct ssq_cursor *cursor);

size_t ssq_cursor_forward(struct ssq_cursor *cursor, size_t count);

size_t ssq_cursor_position(const struct ssq_cursor *cursor);

void ssq_cursor_read(struct ssq_cursor *cursor, void *buffer, size_t count);

float ssq_cursor_read_f32(struct ssq_cursor *cursor);

double ssq_cursor_read_f64(struct ssq_cursor *cursor);

int8_t ssq_cursor_read_i8(struct ssq_cursor *cursor);

int16_t ssq_cursor_read_i16(struct ssq_cursor *cursor);

int32_t ssq_cursor_read_i32(struct ssq_cursor *cursor);

int64_t ssq_cursor_read_i64(struct ssq_cursor *cursor);

uint8_t ssq_cursor_read_u8(struct ssq_cursor *cursor);

uint16_t ssq_cursor_read_u16(struct ssq_cursor *cursor);

uint32_t ssq_cursor_read_u32(struct ssq_cursor *cursor);

uint64_t ssq_cursor_read_u64(struct ssq_cursor *cursor);

bool ssq_cursor_read_bool(struct ssq_cursor *cursor);

char *ssq_cursor_read_string(struct ssq_cursor *cursor, size_t *size);

void ssq_cursor_write(struct ssq_cursor *cursor, const void *data,
                      size_t count);

void ssq_cursor_write_f32(struct ssq_cursor *cursor, float value);

void ssq_cursor_write_f64(struct ssq_cursor *cursor, double value);

void ssq_cursor_write_i8(struct ssq_cursor *cursor, int8_t value);

void ssq_cursor_write_i16(struct ssq_cursor *cursor, int16_t value);

void ssq_cursor_write_i32(struct ssq_cursor *cursor, int32_t value);

void ssq_cursor_write_i64(struct ssq_cursor *cursor, int64_t value);

void ssq_cursor_write_u8(struct ssq_cursor *cursor, uint8_t value);

void ssq_cursor_write_u16(struct ssq_cursor *cursor, uint16_t value);

void ssq_cursor_write_u32(struct ssq_cursor *cursor, uint32_t value);

void ssq_cursor_write_u64(struct ssq_cursor *cursor, uint64_t value);

void ssq_cursor_write_bool(struct ssq_cursor *cursor, bool value);

void ssq_cursor_write_string(struct ssq_cursor *cursor, const char *value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !CURSOR_H */
