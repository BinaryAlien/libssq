#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

uint8_t *read_datagram(const char filename[], size_t *const datagram_len) {
    struct stat stat_buf;
    if (stat(filename, &stat_buf) == -1)
        err(EXIT_FAILURE, "stat: %s", filename);

    *datagram_len = stat_buf.st_size;
    uint8_t *const datagram = malloc(*datagram_len);

    const int fd = open(filename, O_RDONLY);
    if (fd == -1)
        err(EXIT_FAILURE, "open: %s", filename);

    if (read(fd, datagram, *datagram_len) == -1)
        err(EXIT_FAILURE, "read");

    if (close(fd) == -1)
        err(EXIT_FAILURE, "close");

    return datagram;
}
