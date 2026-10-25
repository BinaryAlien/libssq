#include "query.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "buffer.h"
#include "error.h"
#include "packet.h"
#include "server.h"
#include "socket.h"

struct ssq_packet *ssq_query_recv_packet(const ssq_socket descriptor,
                                         struct ssq_error *const error) {
  uint8_t datagram[SSQ_PACKET_SIZE];
  const size_t datagram_size =
      ssq_socket_recv(descriptor, datagram, sizeof(datagram), error);
  if (error->code != SSQE_OK)
    return NULL;
  return ssq_packet_from_buffer(datagram, datagram_size, error);
}

struct ssq_packet **ssq_query_recv_packets(const ssq_socket descriptor,
                                           struct ssq_error *const error) {
  uint8_t packets_count = 1;
  struct ssq_packet **packets = NULL;
  for (uint8_t packets_received = 0; packets_received < packets_count;
       ++packets_received) {
    struct ssq_packet *const packet = ssq_query_recv_packet(descriptor, error);
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

struct ssq_buffer *ssq_query(struct ssq_server *const server,
                             const void *const payload,
                             const size_t payload_size) {
  const ssq_socket descriptor = ssq_socket_open(server);
  if (!ssq_socket_ok(descriptor))
    return NULL;
  struct ssq_buffer *response = NULL;
  if (!ssq_socket_set_timeout(descriptor, &server->timeout,
                              &server->last_error))
    goto exit;
  if (!ssq_socket_send(descriptor, payload, payload_size, &server->last_error))
    goto exit;
  struct ssq_packet **const packets =
      ssq_query_recv_packets(descriptor, &server->last_error);
  if (packets == NULL)
    goto exit;
  if (ssq_packets_check((const struct ssq_packet *const *)packets))
    response = ssq_packets_merge((const struct ssq_packet *const *)packets);
  else
    ssq_error_set(&server->last_error, SSQE_BAD, "Bad packet integrity");
  ssq_packets_free(packets, packets[0]->total);
exit:
  ssq_socket_close(descriptor);
  return response;
}
