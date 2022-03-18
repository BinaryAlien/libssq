#include <stdlib.h>
#include <string.h>
#include "ssq/buf.h"
#include "ssq/helper.h"

SSQ_BUF ssq_buf_init(const void *src, const size_t n) {
    SSQ_BUF buf;

    buf.payload = src;
    buf.pos     = 0;
    buf.size    = n;

    return buf;
}

void ssq_buf_forward(SSQ_BUF *const buf, const size_t n) { buf->pos += n; }
bool ssq_buf_eob(const SSQ_BUF *const buf)               { return buf->pos >= buf->size; }

void ssq_buf_get(void *const dst, SSQ_BUF *const src, const size_t n) {
    if (!ssq_buf_eob(src)) {
        memcpy(dst, src->payload + src->pos, n);
        ssq_buf_forward(src, n);
    }
}

int8_t ssq_buf_get_int8(SSQ_BUF *const buf) {
    int8_t value = 0;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

int16_t ssq_buf_get_int16(SSQ_BUF *const buf) {
    int16_t value = 0;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

int32_t ssq_buf_get_int32(SSQ_BUF *const buf) {
    int32_t value = 0;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

int64_t ssq_buf_get_int64(SSQ_BUF *const buf) {
    int64_t value = 0;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

uint8_t ssq_buf_get_uint8(SSQ_BUF *const buf) {
    uint8_t value = 0;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

uint16_t ssq_buf_get_uint16(SSQ_BUF *const buf) {
    uint16_t value = 0;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

uint32_t ssq_buf_get_uint32(SSQ_BUF *const buf) {
    uint32_t value = 0;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

uint64_t ssq_buf_get_uint64(SSQ_BUF *const buf) {
    uint64_t value = 0;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

float ssq_buf_get_float(SSQ_BUF *const buf) {
    float value = 0.0F;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

double ssq_buf_get_double(SSQ_BUF *const buf) {
    double value = 0.0;
    ssq_buf_get(&value, buf, sizeof (value));
    return value;
}

bool ssq_buf_get_bool(SSQ_BUF *const buf) {
    return ssq_buf_get_uint8(buf) != 0;
}

/* Computes the length of the null-terminated string at the buffer wrapper's current position */
static size_t ssq_buf_strlen(const SSQ_BUF *const buf) {
    size_t len = 0;

    while (buf->pos + len < buf->size && buf->payload[buf->pos + len] != '\0')
        ++len;

    return len;
}

char *ssq_buf_get_string(SSQ_BUF *const buf, size_t *const len) {
    *len = ssq_buf_strlen(buf);

    char *const dst = calloc(*len + 1, sizeof (*dst));

    if (dst != NULL) {
        const char *const src = buf->payload + buf->pos;
        ssq_helper_strncpy(dst, src, *len);
        ssq_buf_forward(buf, *len + 1);
    }

    return dst;
}
