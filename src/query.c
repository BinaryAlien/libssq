#include "ssq/query.h"

#include <stdlib.h>

#include "ssq/packet.h"
#include "ssq/server/private.h"

#ifndef _WIN32
# include <unistd.h>
# define INVALID_SOCKET (-1)
# define SOCKET_ERROR   (-1)
# define closesocket    close
typedef int SOCKET;
#endif /* !_WIN32 */

static int ssq_query_init_socket_timeout(SOCKET sockfd, const SSQ_TIMEOUT *value) {
    int ecode;
    ecode = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&value->recv, sizeof (value->recv));
    if (ecode != SOCKET_ERROR)
        ecode = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&value->send, sizeof (value->send));
    return ecode;
}

static SOCKET ssq_query_init_socket(SSQ_SERVER *server) {
    SOCKET sockfd = INVALID_SOCKET;
    for (struct addrinfo *addr = server->addr_list; addr != NULL; addr = addr->ai_next) {
        sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sockfd == INVALID_SOCKET)
            continue;
        if (connect(sockfd, addr->ai_addr, (int)addr->ai_addrlen) != SOCKET_ERROR)
            break;
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
    }
    if (sockfd == INVALID_SOCKET) {
        ssq_error_set(&server->last_error, SSQE_NO_SOCKET, "Could not create an endpoint for communication");
        return INVALID_SOCKET;
    }
    if (ssq_query_init_socket_timeout(sockfd, &server->timeout) == SOCKET_ERROR) {
#ifdef _WIN32
        ssq_error_set_from_wsa(&server->last_error);
#else /* !_WIN32 */
        ssq_error_set_from_errno(&server->last_error);
#endif /* _WIN32 */
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
    }
    return sockfd;
}

static void ssq_query_send(SOCKET sockfd, const uint8_t payload[], size_t payload_len, SSQ_ERROR *error) {
#ifdef _WIN32
    if (send(sockfd, (const char *)payload, (int)payload_len, 0) == SOCKET_ERROR)
        ssq_error_set_from_wsa(error);
#else /* !_WIN32 */
    if (send(sockfd, payload, payload_len, 0) == SOCKET_ERROR)
        ssq_error_set_from_errno(error);
#endif /* _WIN32 */
}

static SSQ_PACKET **ssq_query_recv(SOCKET sockfd, uint8_t *packet_count, SSQ_ERROR *error) {
    *packet_count = 1;
    SSQ_PACKET **packets = NULL;
    for (uint8_t packets_received = 0; packets_received < *packet_count; ++packets_received) {
        uint8_t datagram[SSQ_PACKET_SIZE];
#ifdef _WIN32
        int bytes_received = recv(sockfd, (char *)datagram, SSQ_PACKET_SIZE, 0);
#else /* !_WIN32 */
        ssize_t bytes_received = recv(sockfd, datagram, SSQ_PACKET_SIZE, 0);
#endif /* _WIN32 */
        if (bytes_received == SOCKET_ERROR) {
#ifdef _WIN32
            ssq_error_set_from_wsa(error);
#else /* !_WIN32 */
            ssq_error_set_from_errno(error);
#endif /* _WIN32 */
            break;
        }
        SSQ_PACKET *packet = ssq_packet_from_datagram(datagram, bytes_received, error);
        if (error->code != SSQE_OK)
            break;
        bool is_first_packet = (packets == NULL);
        if (is_first_packet) {
            *packet_count = packet->total;
            packets = calloc(*packet_count, sizeof (*packets));
            if (packets == NULL) {
                ssq_packet_free(packet);
                ssq_error_set_from_errno(error);
                break;
            }
        }
        packets[packet->number] = packet;
    }
    if (error->code != SSQE_OK && packets != NULL) {
        ssq_packets_free(packets, *packet_count);
        packets = NULL;
    }
    return packets;
}

uint8_t *ssq_query(SSQ_SERVER *server, const uint8_t payload[], size_t payload_len, size_t *response_len) {
    SOCKET sockfd = ssq_query_init_socket(server);
    if (!ssq_server_eok(server))
        return NULL;
    ssq_query_send(sockfd, payload, payload_len, &server->last_error);
    if (!ssq_server_eok(server))
        goto end;
    uint8_t packet_count = 0;
    SSQ_PACKET **packets = ssq_query_recv(sockfd, &packet_count, &server->last_error);
    if (!ssq_server_eok(server))
        goto end;
    const SSQ_PACKET *const *packets_readonly = (const SSQ_PACKET *const *)packets;
    uint8_t *response = NULL;
    if (ssq_packets_check_integrity(packets_readonly, packet_count))
        response = ssq_packets_to_response(packets_readonly, packet_count, response_len, &server->last_error);
    else
        ssq_error_set(&server->last_error, SSQE_INVALID_RESPONSE, "Packet ID mismatch");
    ssq_packets_free(packets, packet_count);
end:
    closesocket(sockfd);
    return response;
}
