#include "query.h"

#include <stddef.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else /* !_WIN32 */
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif /* _WIN32 */

#include "packet.h"
#include "server.h"

#ifdef _WIN32
typedef SOCKET ssq_socket;
#else  /* !_WIN32 */
typedef int ssq_socket;
#endif /* _WIN32 */

void ssq_socket_close(const ssq_socket descriptor) {
#ifdef _WIN32
  closesocket(descriptor);
#else  /* !_WIN32 */
  close(descriptor);
#endif /* _WIN32 */
}

ssq_socket ssq_socket_open(struct ssq_server *const server) {
  ssq_socket descriptor = INVALID_SOCKET;
  for (const struct addrinfo *address = server->addresses; address != NULL;
       address = address->ai_next) {
    descriptor =
        socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (descriptor == INVALID_SOCKET)
      continue;
    if (connect(descriptor, address->ai_addr, address->ai_addrlen) !=
        SOCKET_ERROR)
      break;
    ssq_socket_close(descriptor);
    descriptor = INVALID_SOCKET;
  }
  if (descriptor == INVALID_SOCKET)
    ssq_error_set(&server->last_error, SSQE_MUTE,
                  "Could not open an endpoint for communication");
  return descriptor;
}

static bool ssq_socket_configure(const ssq_socket descriptor,
                                 const struct ssq_timeout *const timeout,
                                 struct ssq_error *const error) {
  if (setsockopt(descriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout->recv,
                 sizeof(timeout->recv)) == SOCKET_ERROR) {
    ssq_error_set_from_errno(error);
    return false;
  }
  if (setsockopt(descriptor, SOL_SOCKET, SO_SNDTIMEO, &timeout->send,
                 sizeof(timeout->send)) == SOCKET_ERROR) {
    ssq_error_set_from_errno(error);
    return false;
  }
  return true;
}

bool ssq_query_send(const ssq_socket descriptor, const uint8_t *const payload,
                    const size_t size, struct ssq_error *const error) {
#ifdef _WIN32
  if (send(descriptor, (const char *)payload, (int)payload_size, 0) ==
      SOCKET_ERROR) {
    ssq_error_set_from_wsa(error);
    return false;
  }
#else  /* !_WIN32 */
  if (send(descriptor, payload, size, 0) == SOCKET_ERROR) {
    ssq_error_set_from_errno(error);
    return false;
  }
#endif /* _WIN32 */
  return true;
}

struct ssq_packet **ssq_query_recv(const ssq_socket descriptor,
                                   struct ssq_error *const error) {
  uint8_t packets_count = 1;
  struct ssq_packet **packets = NULL;
  for (uint8_t packets_received = 0; packets_received < packets_count;
       ++packets_received) {
    uint8_t datagram[SSQ_PACKET_SIZE];
#ifdef _WIN32
    const int datagram_size =
        recv(descriptor, (char *)datagram, SSQ_PACKET_SIZE, 0);
#else  /* !_WIN32 */
    const ssize_t datagram_size =
        recv(descriptor, datagram, SSQ_PACKET_SIZE, 0);
#endif /* _WIN32 */
    if (datagram_size == SOCKET_ERROR) {
#ifdef _WIN32
      ssq_error_set_from_wsa(error);
#else  /* !_WIN32 */
      ssq_error_set_from_errno(error);
#endif /* _WIN32 */
      break;
    }
    struct ssq_packet *const packet =
        ssq_packet_from_buffer(datagram, datagram_size, error);
    if (packet == NULL)
      break;
    if (packets == NULL) {
      packets_count = packet->total;
      packets = calloc(packets_count, sizeof(*packets));
      if (packets == NULL) {
        ssq_error_set_from_errno(error);
        ssq_packet_free(packet);
        break;
      }
    }
    packets[packet->number] = packet;
  }
  if (error->code != SSQE_OK && packets != NULL) {
    ssq_packets_free(packets, packets_count);
    packets = NULL;
  }
  return packets;
}

struct ssq_response *ssq_query(struct ssq_server *const server,
                               const uint8_t *const payload,
                               const size_t size) {
  const ssq_socket descriptor = ssq_socket_open(server);
  if (descriptor == INVALID_SOCKET)
    return NULL;
  struct ssq_response *response = NULL;
  if (!ssq_socket_configure(descriptor, &server->timeout, &server->last_error))
    goto exit;
  if (!ssq_query_send(descriptor, payload, size, &server->last_error))
    goto exit;
  struct ssq_packet **const packets =
      ssq_query_recv(descriptor, &server->last_error);
  if (packets == NULL)
    goto exit;
  response =
      ssq_response_from_packets((const struct ssq_packet *const *)packets);
  ssq_packets_free(packets, packets[0]->total);
exit:
  ssq_socket_close(descriptor);
  return response;
}
