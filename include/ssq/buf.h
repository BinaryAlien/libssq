#ifndef SSQ_BUF_H
#define SSQ_BUF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ssq_buf {
    const char *payload;
    size_t      pos;
    size_t      size;
} SSQ_BUF;

/**
 * Initializes a stack-allocated buffer wrapper.
 * @param src the source buffer to use
 * @param n   the length the buffer may read in the source buffer
 * @return    a newly stack-allocated buffer wrapper
 */
SSQ_BUF ssq_buf_init(const void *src, size_t n);

/**
 * Forwards N bytes into a buffer wrapper.
 * @param buf the buffer wrapper
 * @param n   number of bytes to forward in `buf`
 */
void ssq_buf_forward(SSQ_BUF *buf, size_t n);

/**
 * Determines if the end of a buffer wrapper was reached.
 * @param buf the buffer wrapper
 * @return true if the end of the buffer wrapper was reached
 */
bool ssq_buf_eob(const SSQ_BUF *buf);

/**
 * Reads N bytes of memory from a source buffer wrapper into a destination memory space.
 * @param dst the destination memory space
 * @param src the source buffer wrapper
 * @param n   number of bytes to read from `src` into` dst`
 */
void ssq_buf_get(void *dst, SSQ_BUF *src, size_t n);

/**
 * Reads an 8-bit signed integer from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the 8-bit signed integer at the buffer wrapper's current position
 */
int8_t ssq_buf_get_int8(SSQ_BUF *buf);

/**
 * Reads a 16-bit signed integer from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the 16-bit signed integer at the buffer wrapper's current position
 */
int16_t ssq_buf_get_int16(SSQ_BUF *buf);

/**
 * Reads a 32-bit signed integer from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the 32-bit signed integer at the buffer wrapper's current position
 */
int32_t ssq_buf_get_int32(SSQ_BUF *buf);

/**
 * Reads a 64-bit signed integer from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the 64-bit signed integer at the buffer wrapper's current position
 */
int64_t ssq_buf_get_int64(SSQ_BUF *buf);

/**
 * Reads an 8-bit unsigned integer from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the 8-bit unsigned integer at the buffer wrapper's current position
 */
uint8_t ssq_buf_get_uint8(SSQ_BUF *buf);

/**
 * Reads a 16-bit unsigned integer from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the 16-bit unsigned integer at the buffer wrapper's current position
 */
uint16_t ssq_buf_get_uint16(SSQ_BUF *buf);

/**
 * Reads a 32-bit unsigned integer from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the 32-bit unsigned integer at the buffer wrapper's current position
 */
uint32_t ssq_buf_get_uint32(SSQ_BUF *buf);

/**
 * Reads a 64-bit unsigned integer from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the 64-bit unsigned integer at the buffer wrapper's current position
 */
uint64_t ssq_buf_get_uint64(SSQ_BUF *buf);

/**
 * Reads a float from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the float at the buffer wrapper's current position
 */
float ssq_buf_get_float(SSQ_BUF *buf);

/**
 * Reads a double from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the double at the buffer wrapper's current position
 */
double ssq_buf_get_double(SSQ_BUF *buf);

/**
 * Reads a boolean from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @returns the boolean at the buffer wrapper's current position
 */
bool ssq_buf_get_bool(SSQ_BUF *buf);

/**
 * Reads a null-terminated string from a buffer wrapper.
 * @param buf the buffer wrapper to read from
 * @param len where to store the length of the string
 * @return a dynamically-allocated copy of the string at the buffer wrapper's current position
 */
char *ssq_buf_get_string(SSQ_BUF *buf, size_t *len);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_BUF_H */
