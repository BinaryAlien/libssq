#ifndef SSQ_BUF_H
#define SSQ_BUF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ssq_buf {
    const uint8_t *payload;
    size_t         payload_len;
    size_t         cursor;
} SSQ_BUF;

/**
 * Initializes a byte buffer.
 *
 * @param src source buffer to read
 * @param n   length of the source buffer
 *
 * @return new byte buffer to read from `src'
 */
SSQ_BUF ssq_buf_init(const void *src, size_t n);

/**
 * Forwards N bytes into a byte buffer.
 *
 * @param buf byte buffer
 * @param n   number of bytes to forward in the byte buffer
 */
void ssq_buf_forward(SSQ_BUF *buf, size_t n);

/**
 * Determines if the end of a byte buffer was reached.
 * @param buf byte buffer
 * @return true if there is no more bytes available to read
 */
bool ssq_buf_eof(const SSQ_BUF *buf);

/**
 * Computes the number of bytes available to read from a byte buffer.
 * @param buf byte buffer
 * @return number of bytes available to read from the byte buffer
 */
size_t ssq_buf_available(const SSQ_BUF *buf);

/**
 * Reads N bytes from a source byte buffer into a destination memory space.
 * If the number of bytes to read exceeds the number of bytes available
 * to read: fills the destination memory space with zeroes.
 *
 * @param dst destination memory space
 * @param src source byte buffer
 * @param n   number of bytes to read from `src' into `dst'
 */
void ssq_buf_get(void *dst, SSQ_BUF *src, size_t n);

/**
 * Reads an 8-bit signed integer from a byte buffer.
 * @param buf byte buffer to read from
 * @return 8-bit signed integer at the byte buffer's current position
 */
int8_t ssq_buf_get_int8(SSQ_BUF *buf);

/**
 * Reads a 16-bit signed integer from a byte buffer.
 * @param buf byte buffer to read from
 * @return 16-bit signed integer at the byte buffer's current position
 */
int16_t ssq_buf_get_int16(SSQ_BUF *buf);

/**
 * Reads a 32-bit signed integer from a byte buffer.
 * @param buf byte buffer to read from
 * @return 32-bit signed integer at the byte buffer's current position
 */
int32_t ssq_buf_get_int32(SSQ_BUF *buf);

/**
 * Reads a 64-bit signed integer from a byte buffer.
 * @param buf byte buffer to read from
 * @return 64-bit signed integer at the byte buffer's current position
 */
int64_t ssq_buf_get_int64(SSQ_BUF *buf);

/**
 * Reads an 8-bit unsigned integer from a byte buffer.
 * @param buf byte buffer to read from
 * @return 8-bit unsigned integer at the byte buffer's current position
 */
uint8_t ssq_buf_get_uint8(SSQ_BUF *buf);

/**
 * Reads a 16-bit unsigned integer from a byte buffer.
 * @param buf byte buffer to read from
 * @return 16-bit unsigned integer at the byte buffer's current position
 */
uint16_t ssq_buf_get_uint16(SSQ_BUF *buf);

/**
 * Reads a 32-bit unsigned integer from a byte buffer.
 * @param buf byte buffer to read from
 * @return 32-bit unsigned integer at the byte buffer's current position
 */
uint32_t ssq_buf_get_uint32(SSQ_BUF *buf);

/**
 * Reads a 64-bit unsigned integer from a byte buffer.
 * @param buf byte buffer to read from
 * @return 64-bit unsigned integer at the byte buffer's current position
 */
uint64_t ssq_buf_get_uint64(SSQ_BUF *buf);

/**
 * Reads a float from a byte buffer.
 * @param buf byte buffer to read from
 * @return float at the byte buffer's current position
 */
float ssq_buf_get_float(SSQ_BUF *buf);

/**
 * Reads a double from a byte buffer.
 * @param buf byte buffer to read from
 * @return double at the byte buffer's current position
 */
double ssq_buf_get_double(SSQ_BUF *buf);

/**
 * Reads a boolean from a byte buffer.
 * @param buf byte buffer to read from
 * @return boolean at the byte buffer's current position
 */
bool ssq_buf_get_bool(SSQ_BUF *buf);

/**
 * Reads a null-terminated string from a byte buffer.
 *
 * @param buf byte buffer to read from
 * @param len where to store the length of the string
 *
 * @return dynamically-allocated copy of the string at the byte buffer's current position
 */
char *ssq_buf_get_string(SSQ_BUF *buf, size_t *len);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_BUF_H */
