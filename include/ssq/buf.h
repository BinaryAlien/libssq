#ifndef SSQ_BUF_H
#define SSQ_BUF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ssq_buf {
    const uint8_t *payload;
    size_t         size;
    size_t         cursor;
} SSQ_BUF;

void     ssq_buf_init(SSQ_BUF *buf, const void *src, size_t n);

void     ssq_buf_forward(SSQ_BUF *buf, size_t n);
bool     ssq_buf_eof(const SSQ_BUF *buf);
size_t   ssq_buf_available(const SSQ_BUF *buf);

void     ssq_buf_read(void *dest, SSQ_BUF *src, size_t n);

int8_t   ssq_buf_read_int8(SSQ_BUF *buf);
int16_t  ssq_buf_read_int16(SSQ_BUF *buf);
int32_t  ssq_buf_read_int32(SSQ_BUF *buf);
int64_t  ssq_buf_read_int64(SSQ_BUF *buf);

uint8_t  ssq_buf_read_uint8(SSQ_BUF *buf);
uint16_t ssq_buf_read_uint16(SSQ_BUF *buf);
uint32_t ssq_buf_read_uint32(SSQ_BUF *buf);
uint64_t ssq_buf_read_uint64(SSQ_BUF *buf);

float    ssq_buf_read_float(SSQ_BUF *buf);
double   ssq_buf_read_double(SSQ_BUF *buf);

bool     ssq_buf_read_bool(SSQ_BUF *buf);

char    *ssq_buf_read_string(SSQ_BUF *buf, size_t *len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_BUF_H */
