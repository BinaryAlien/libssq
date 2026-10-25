#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_buffer {
  uint8_t *data;
  size_t size;
};

struct ssq_buffer *ssq_buffer_alloc(size_t size);

void ssq_buffer_free(struct ssq_buffer *buffer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !BUFFER_H */
