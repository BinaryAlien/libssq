#include <stdlib.h>
#include <string.h>
#include "ssq/buf.h"
#include "ssq/helper.h"

SSQ_BUF ssq_buf_init(const void *src, const size_t n) {
    SSQ_BUF buf;

    buf.payload     = src;
    buf.cursor      = 0;
    buf.payload_len = n;

    return buf;
}

void ssq_buf_forward(SSQ_BUF *const buf, const size_t n) {
    buf->cursor += n;
}

bool ssq_buf_eof(const SSQ_BUF *const buf) {
    return buf->cursor >= buf->payload_len;
}

size_t ssq_buf_available(const SSQ_BUF *const buf) {
    return ssq_buf_eof(buf) ? 0 : (buf->payload_len - buf->cursor);
}

void ssq_buf_get(void *const dst, SSQ_BUF *const src, const size_t n) {
    if (n <= ssq_buf_available(src)) {
        memcpy(dst, src->payload + src->cursor, n);
        ssq_buf_forward(src, n);
    } else {
        memset(dst, 0, n);
    }
}

int8_t ssq_buf_get_int8(SSQ_BUF *const buf) {
    int8_t value;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

int16_t ssq_buf_get_int16(SSQ_BUF *const buf) {
    int16_t value;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

int32_t ssq_buf_get_int32(SSQ_BUF *const buf) {
    int32_t value;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

int64_t ssq_buf_get_int64(SSQ_BUF *const buf) {
    int64_t value;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

uint8_t ssq_buf_get_uint8(SSQ_BUF *const buf) {
    uint8_t value;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

uint16_t ssq_buf_get_uint16(SSQ_BUF *const buf) {
    uint16_t value;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

uint32_t ssq_buf_get_uint32(SSQ_BUF *const buf) {
    uint32_t value;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

uint64_t ssq_buf_get_uint64(SSQ_BUF *const buf) {
    uint64_t value;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

float ssq_buf_get_float(SSQ_BUF *const buf) {
    float value;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

double ssq_buf_get_double(SSQ_BUF *const buf) {
    double value;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

bool ssq_buf_get_bool(SSQ_BUF *const buf) {
    return ssq_buf_get_uint8(buf) != 0;
}

/**
 * Computes the length of a null-terminated string at a byte buffer's current position.
 * If there is no null terminator, the string is considered to end at the end of the byte buffer.
 *
 * @param buf byte buffer
 *
 * @returns length of the null-terminated string at the byte buffer's current position
 */
static size_t ssq_buf_get_string_len(const SSQ_BUF *const buf) {
    size_t len = 0;

    while (len < ssq_buf_available(buf) && buf->payload[buf->cursor + len] != '\0')
        ++len;

    return len;
}

char *ssq_buf_get_string(SSQ_BUF *const buf, size_t *const len) {
    *len = ssq_buf_get_string_len(buf);

    char *const dst = calloc(*len + 1, sizeof (*dst));

    if (dst != NULL) {
        const char *const src = (const char *)(buf->payload + buf->cursor);
        ssq_helper_strncpy(dst, src, *len);
        ssq_buf_forward(buf, *len + 1);
    }

    return dst;
}
