#include "ssq/query.h"

#include <stdlib.h>

#include "ssq/packet.h"

#ifdef _WIN32
# include <winsock2.h>
#else /* !_WIN32 */
# include <unistd.h>
# define INVALID_SOCKET (-1)
# define SOCKET_ERROR   (-1)
# define closesocket    close
typedef int SOCKET;
#endif /* _WIN32 */

static SOCKET ssq_query_init_socket(SSQ_SERVER *server) {
    SOCKET sockfd = INVALID_SOCKET;
    for (struct addrinfo *addr = server->addr_list; addr != NULL; addr = addr->ai_next) {
        sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sockfd == INVALID_SOCKET) {
            continue;
        } else if (connect(sockfd, addr->ai_addr, (int)addr->ai_addrlen) != SOCKET_ERROR) {
            break;
        } else {
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
        }
    }
    if (sockfd != INVALID_SOCKET) {
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&(server->timeout_recv), sizeof (server->timeout_recv)) == SOCKET_ERROR ||
            setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&(server->timeout_send), sizeof (server->timeout_send)) == SOCKET_ERROR) {
#ifdef _WIN32
            ssq_error_set_from_wsa(&(server->last_error));
#else /* !_WIN32 */
            ssq_error_set_from_errno(&(server->last_error));
#endif /* _WIN32 */
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
        }
    } else {
        ssq_error_set(&(server->last_error), SSQE_NO_SOCKET, "Could not create a socket");
    }
    return sockfd;
}

static void ssq_query_send(SOCKET sockfd, const uint8_t payload[], size_t payload_len, SSQ_ERROR *err) {
#ifdef _WIN32
    if (send(sockfd, (const char *)payload, (int)payload_len, 0) == SOCKET_ERROR)
        ssq_error_set_from_wsa(err);
#else /* !_WIN32 */
    if (send(sockfd, payload, payload_len, 0) == SOCKET_ERROR)
        ssq_error_set_from_errno(err);
#endif /* _WIN32 */
}

static SSQ_PACKET **ssq_query_recv(SOCKET sockfd, uint8_t *packet_count, SSQ_ERROR *err) {
    *packet_count = 1;
    SSQ_PACKET **packets = NULL;
    for (uint8_t packets_received = 0; packets_received < *packet_count; ++packets_received) {
        uint8_t datagram[SSQ_PACKET_SIZE];
#ifdef _WIN32
        int bytes_received = recv(sockfd, (char *)datagram, SSQ_PACKET_SIZE, 0);
#else /* !_WIN32 */
        ssize_t bytes_received = recv(sockfd, datagram, SSQ_PACKET_SIZE, 0);
#endif /* _WIN32 */
        if (bytes_received != SOCKET_ERROR) {
            SSQ_PACKET *packet = ssq_packet_from_datagram(datagram, bytes_received, err);
            if (err->code != SSQE_OK)
                break;
            bool is_first_packet = (packets == NULL);
            if (is_first_packet) {
                *packet_count = packet->total;
                packets = calloc(*packet_count, sizeof (SSQ_PACKET *));
                if (packets == NULL) {
                    ssq_error_set_from_errno(err);
                    break;
                }
            }
            packets[packet->number] = packet;
        } else {
#ifdef _WIN32
            ssq_error_set_from_wsa(err);
#else /* !_WIN32 */
            ssq_error_set_from_errno(err);
#endif /* _WIN32 */
            break;
        }
    }
    if (err->code != SSQE_OK && packets != NULL) {
        ssq_packets_free(packets, *packet_count);
        packets = NULL;
    }
    return packets;
}

uint8_t *ssq_query(SSQ_SERVER *server, const uint8_t payload[], size_t payload_len, size_t *response_len) {
    uint8_t *response = NULL;
    SOCKET sockfd = ssq_query_init_socket(server);
    if (!ssq_server_ok(server))
        return NULL;
    ssq_query_send(sockfd, payload, payload_len, &(server->last_error));
    if (!ssq_server_ok(server))
        goto end;
    uint8_t packet_count = 0;
    SSQ_PACKET **packets = ssq_query_recv(sockfd, &packet_count, &(server->last_error));
    if (!ssq_server_ok(server))
        goto end;
    const SSQ_PACKET *const *packets_readonly = (const SSQ_PACKET *const *)packets;
    if (ssq_packets_check_integrity(packets_readonly, packet_count))
        response = ssq_packets_to_response(packets_readonly, packet_count, response_len, &(server->last_error));
    else
        ssq_error_set(&(server->last_error), SSQE_INVALID_RESPONSE, "Packet ID mismatch");
    ssq_packets_free(packets, packet_count);
end:
    closesocket(sockfd);
    return response;
}
