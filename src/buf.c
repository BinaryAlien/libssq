#include "ssq/buf.h"

#include <stdlib.h>
#include <string.h>

#include "ssq/helper.h"

void ssq_buf_init(SSQ_BUF *buf, const void *src, size_t n) {
    buf->payload = src;
    buf->size = n;
    buf->cursor = 0;
}

void ssq_buf_forward(SSQ_BUF *buf, size_t n) {
    buf->cursor += n;
}

bool ssq_buf_eof(const SSQ_BUF *buf) {
    return buf->cursor >= buf->size;
}

size_t ssq_buf_available(const SSQ_BUF *buf) {
    return ssq_buf_eof(buf) ? 0 : (buf->size - buf->cursor);
}

void ssq_buf_read(void *dest, SSQ_BUF *src, size_t n) {
    if (n <= ssq_buf_available(src)) {
        memcpy(dest, src->payload + src->cursor, n);
        ssq_buf_forward(src, n);
    } else {
        memset(dest, 0, n);
    }
}

int8_t ssq_buf_read_int8(SSQ_BUF *buf) {
    int8_t value;
    ssq_buf_read(&value, buf, sizeof (value));
    return value;
}

int16_t ssq_buf_read_int16(SSQ_BUF *buf) {
    int16_t value;
    ssq_buf_read(&value, buf, sizeof (value));
    return value;
}

int32_t ssq_buf_read_int32(SSQ_BUF *buf) {
    int32_t value;
    ssq_buf_read(&value, buf, sizeof (value));
    return value;
}

int64_t ssq_buf_read_int64(SSQ_BUF *buf) {
    int64_t value;
    ssq_buf_read(&value, buf, sizeof (value));
    return value;
}

uint8_t ssq_buf_read_uint8(SSQ_BUF *buf) {
    uint8_t value;
    ssq_buf_read(&value, buf, sizeof (value));
    return value;
}

uint16_t ssq_buf_read_uint16(SSQ_BUF *buf) {
    uint16_t value;
    ssq_buf_read(&value, buf, sizeof (value));
    return value;
}

uint32_t ssq_buf_read_uint32(SSQ_BUF *buf) {
    uint32_t value;
    ssq_buf_read(&value, buf, sizeof (value));
    return value;
}

uint64_t ssq_buf_read_uint64(SSQ_BUF *buf) {
    uint64_t value;
    ssq_buf_read(&value, buf, sizeof (value));
    return value;
}

float ssq_buf_read_float(SSQ_BUF *buf) {
    float value;
    ssq_buf_read(&value, buf, sizeof (value));
    return value;
}

double ssq_buf_read_double(SSQ_BUF *buf) {
    double value;
    ssq_buf_read(&value, buf, sizeof (value));
    return value;
}

bool ssq_buf_read_bool(SSQ_BUF *buf) {
    return ssq_buf_read_uint8(buf);
}

static size_t ssq_buf_strlen(const SSQ_BUF *buf) {
    size_t len = 0;
    while (len < ssq_buf_available(buf) && buf->payload[buf->cursor + len] != '\0')
        ++len;
    return len;
}

char *ssq_buf_read_string(SSQ_BUF *buf, size_t *len) {
    *len = ssq_buf_strlen(buf);
    char *dest = calloc(*len + 1, sizeof (*dest));
    if (dest != NULL) {
        const char *src = (const char *)(buf->payload + buf->cursor);
        ssq_helper_strncpy(dest, src, *len);
        ssq_buf_forward(buf, *len + 1);
    }
    return dest;
}
