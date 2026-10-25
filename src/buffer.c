#include "buffer.h"

#include <stddef.h>
#include <stdlib.h>

struct ssq_buffer *ssq_buffer_alloc(const size_t size) {
  struct ssq_buffer *buffer = malloc(sizeof(*buffer));
  if (buffer == NULL)
    return NULL;
  buffer->size = size;
  buffer->data = malloc(buffer->size);
  if (buffer->data == NULL) {
    free(buffer);
    return NULL;
  }
  return buffer;
}

void ssq_buffer_free(struct ssq_buffer *const buffer) {
  if (buffer == NULL)
    return;
  free(buffer->data);
  free(buffer);
}
