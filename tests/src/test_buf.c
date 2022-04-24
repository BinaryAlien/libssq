#include <criterion/criterion.h>
#include <float.h>
#include <limits.h>
#include "ssq/buf.h"

Test(buf, init) {
    const char payload[] = "Hello, world!";

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    cr_expect_eq((void *)buf.payload, (void *)payload);
    cr_expect_eq(buf.payload_len, 14);
    cr_expect_eq(buf.cursor, 0);
}

Test(buf, forward) {
    const char payload[] = "Hello, world!";

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    ssq_buf_forward(&buf, 4);
    cr_expect_eq(buf.cursor, 4);

    ssq_buf_forward(&buf, 8);
    cr_expect_eq(buf.cursor, 12);
}

Test(buf, eof) {
    const char payload[] = "Hello, world!";

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    ssq_buf_forward(&buf, 999);
    cr_expect(ssq_buf_eof(&buf));
}

Test(buf, available) {
    const char payload[] = "Hello, world!";

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    cr_expect_eq(ssq_buf_available(&buf), 14);
    ssq_buf_forward(&buf, 4);
    cr_expect_eq(ssq_buf_available(&buf), 10);
    ssq_buf_forward(&buf, 999);
    cr_expect_eq(ssq_buf_available(&buf), 0);
}

Test(buf, get) {
    const uint8_t payload[] = { 0x21, 0x42 };

    uint8_t p = 0xFF;
    uint8_t q = 0xFF;
    uint8_t r = 0xFF;

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    ssq_buf_get(&p, &buf, 1);
    ssq_buf_get(&q, &buf, 1);
    ssq_buf_get(&r, &buf, 1);

    cr_expect_eq(p, 0x21);
    cr_expect_eq(q, 0x42);
    cr_expect_eq(r, 0);

    cr_expect_eq(buf.cursor, 2);
    cr_expect(ssq_buf_eof(&buf));
}

Test(buf, get_int8) {
    const uint8_t payload[] = { 0x80, 0x7F, 0x00, 0x01 };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    int8_t p = ssq_buf_get_int8(&buf);
    int8_t q = ssq_buf_get_int8(&buf);
    int8_t r = ssq_buf_get_int8(&buf);
    int8_t s = ssq_buf_get_int8(&buf);
    int8_t t = ssq_buf_get_int8(&buf);

    cr_expect_eq(p, INT8_MIN);
    cr_expect_eq(q, INT8_MAX);
    cr_expect_eq(r, 0);
    cr_expect_eq(s, 1);
    cr_expect_eq(t, 0);

    cr_expect_eq(buf.cursor, 4 * sizeof (int8_t));
    cr_expect(ssq_buf_eof(&buf));
}

Test(buf, get_int16) {
    const uint8_t payload[] = {
        0x00, 0x80,
        0xFF, 0x7F,
        0x00, 0x00,
        0x01, 0x00,
        0xCA
    };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    int16_t p = ssq_buf_get_int16(&buf);
    int16_t q = ssq_buf_get_int16(&buf);
    int16_t r = ssq_buf_get_int16(&buf);
    int16_t s = ssq_buf_get_int16(&buf);
    int16_t t = ssq_buf_get_int16(&buf);

    cr_expect_eq(p, INT16_MIN);
    cr_expect_eq(q, INT16_MAX);
    cr_expect_eq(r, 0);
    cr_expect_eq(s, 1);
    cr_expect_eq(t, 0);

    cr_expect_eq(buf.cursor, 4 * sizeof (int16_t));
    cr_expect(!ssq_buf_eof(&buf));
}

Test(buf, get_int32) {
    const uint8_t payload[] = {
        0x00, 0x00, 0x00, 0x80,
        0xFF, 0xFF, 0xFF, 0x7F,
        0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00,
        0xCA, 0xFE, 0x42
    };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    int32_t p = ssq_buf_get_int32(&buf);
    int32_t q = ssq_buf_get_int32(&buf);
    int32_t r = ssq_buf_get_int32(&buf);
    int32_t s = ssq_buf_get_int32(&buf);
    int32_t t = ssq_buf_get_int32(&buf);

    cr_expect_eq(p, INT32_MIN);
    cr_expect_eq(q, INT32_MAX);
    cr_expect_eq(r, 0);
    cr_expect_eq(s, 1);
    cr_expect_eq(t, 0);

    cr_expect_eq(buf.cursor, 4 * sizeof (int32_t));
    cr_expect(!ssq_buf_eof(&buf));
}

Test(buf, get_int64) {
    const uint8_t payload[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xCA, 0xFE, 0x42
    };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    int64_t p = ssq_buf_get_int64(&buf);
    int64_t q = ssq_buf_get_int64(&buf);
    int64_t r = ssq_buf_get_int64(&buf);
    int64_t s = ssq_buf_get_int64(&buf);
    int64_t t = ssq_buf_get_int64(&buf);

    cr_expect_eq(p, INT64_MIN);
    cr_expect_eq(q, INT64_MAX);
    cr_expect_eq(r, 0);
    cr_expect_eq(s, 1);
    cr_expect_eq(t, 0);

    cr_expect_eq(buf.cursor, 4 * sizeof (int64_t));
    cr_expect(!ssq_buf_eof(&buf));
}

Test(buf, get_uint8) {
    const uint8_t payload[] = { 0xFF, 0x00, 0x01 };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    uint8_t p = ssq_buf_get_uint8(&buf);
    uint8_t q = ssq_buf_get_uint8(&buf);
    uint8_t r = ssq_buf_get_uint8(&buf);
    uint8_t s = ssq_buf_get_uint8(&buf);

    cr_expect_eq(p, UINT8_MAX);
    cr_expect_eq(q, 0);
    cr_expect_eq(r, 1);
    cr_expect_eq(s, 0);

    cr_expect_eq(buf.cursor, 3 * sizeof (uint8_t));
    cr_expect(ssq_buf_eof(&buf));
}

Test(buf, get_uint16) {
    const uint8_t payload[] = {
        0xFF, 0xFF,
        0x00, 0x00,
        0x01, 0x00,
        0xCA
    };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    uint16_t p = ssq_buf_get_uint16(&buf);
    uint16_t q = ssq_buf_get_uint16(&buf);
    uint16_t r = ssq_buf_get_uint16(&buf);
    uint16_t s = ssq_buf_get_uint16(&buf);

    cr_expect_eq(p, UINT16_MAX);
    cr_expect_eq(q, 0);
    cr_expect_eq(r, 1);
    cr_expect_eq(s, 0);

    cr_expect_eq(buf.cursor, 3 * sizeof (uint16_t));
    cr_expect(!ssq_buf_eof(&buf));
}

Test(buf, get_uint32) {
    const uint8_t payload[] = {
        0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00,
        0xCA, 0xFE, 0x42
    };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    uint32_t p = ssq_buf_get_uint32(&buf);
    uint32_t q = ssq_buf_get_uint32(&buf);
    uint32_t r = ssq_buf_get_uint32(&buf);
    uint32_t s = ssq_buf_get_uint32(&buf);

    cr_expect_eq(p, UINT32_MAX);
    cr_expect_eq(q, 0);
    cr_expect_eq(r, 1);
    cr_expect_eq(s, 0);

    cr_expect_eq(buf.cursor, 3 * sizeof (uint32_t));
    cr_expect(!ssq_buf_eof(&buf));
}

Test(buf, get_uint64) {
    const uint8_t payload[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xCA, 0xFE, 0x42
    };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    uint64_t p = ssq_buf_get_uint64(&buf);
    uint64_t q = ssq_buf_get_uint64(&buf);
    uint64_t r = ssq_buf_get_uint64(&buf);
    uint64_t s = ssq_buf_get_uint64(&buf);

    cr_expect_eq(p, UINT64_MAX);
    cr_expect_eq(q, 0);
    cr_expect_eq(r, 1);
    cr_expect_eq(s, 0);

    cr_expect_eq(buf.cursor, 3 * sizeof (uint64_t));
    cr_expect(!ssq_buf_eof(&buf));
}

Test(buf, get_float) {
    const uint8_t payload[] = {
        0x00, 0x00, 0x00, 0x00, // 0
        0x00, 0x00, 0x80, 0x3F, // 1
        0x00, 0x20, 0x00, 0xC3, // -128.125
        0x00, 0x20, 0x00, 0x4B, // 8396800
        0x42
    };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    float p = ssq_buf_get_float(&buf);
    float q = ssq_buf_get_float(&buf);
    float r = ssq_buf_get_float(&buf);
    float s = ssq_buf_get_float(&buf);
    float t = ssq_buf_get_float(&buf);

    cr_expect_float_eq(p, 0.0F, 1e0);
    cr_expect_float_eq(q, 1.0F, 1e0);
    cr_expect_float_eq(r, -128.125F, 1e0);
    cr_expect_float_eq(s, 8396800.0F, 1e0);
    cr_expect_float_eq(t, 0.0F, 1e0);

    cr_expect_eq(buf.cursor, 4 * sizeof (float));
    cr_expect(!ssq_buf_eof(&buf));
}

Test(buf, get_double) {
    const uint8_t payload[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3F, // 1
        0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x60, 0xC0, // -128.125
        0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x60, 0x41, // 8396800
        0x42
    };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    double p = ssq_buf_get_double(&buf);
    double q = ssq_buf_get_double(&buf);
    double r = ssq_buf_get_double(&buf);
    double s = ssq_buf_get_double(&buf);
    double t = ssq_buf_get_double(&buf);

    cr_expect_float_eq(p, 0.0, 1e0);
    cr_expect_float_eq(q, 1.0, 1e0);
    cr_expect_float_eq(r, -128.125, 1e0);
    cr_expect_float_eq(s, 8396800.0, 1e0);
    cr_expect_float_eq(t, 0.0, 1e0);

    cr_expect_eq(buf.cursor, 4 * sizeof (double));
    cr_expect(!ssq_buf_eof(&buf));
}

Test(buf, get_boolean) {
    const uint8_t payload[] = { 0x00, 0x01, 0x03, 0xFF };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    bool p = ssq_buf_get_bool(&buf);
    bool q = ssq_buf_get_bool(&buf);
    bool r = ssq_buf_get_bool(&buf);
    bool s = ssq_buf_get_bool(&buf);
    bool t = ssq_buf_get_bool(&buf);

    cr_expect_eq(p, false);
    cr_expect_eq(q, true);
    cr_expect_eq(r, true);
    cr_expect_eq(s, true);
    cr_expect_eq(t, false);

    cr_expect_eq(buf.cursor, 4 * sizeof (bool));
    cr_expect(ssq_buf_eof(&buf));
}

Test(buf, get_string) {
    const uint8_t payload[] = {
        'H', 'e', 'l', 'l', 'o', '\0',
        'W', 'o', 'r', 'l', 'd', 0xFF
    };

    SSQ_BUF buf = ssq_buf_init(payload, sizeof (payload));

    size_t s1_len = 0;
    char  *s1     = ssq_buf_get_string(&buf, &s1_len);

    cr_expect_eq(s1_len, 5);
    cr_expect_str_eq(s1, "Hello");
    cr_expect_eq(buf.cursor, 6);
    cr_expect(!ssq_buf_eof(&buf));

    size_t s2_len = 0;
    char  *s2     = ssq_buf_get_string(&buf, &s2_len);

    cr_expect_eq(s2_len, 6);
    cr_expect_str_eq(s2, "World\xFF");
    cr_expect_eq(buf.cursor, 13);
    cr_expect(ssq_buf_eof(&buf));

    free(s1);
    free(s2);
}
