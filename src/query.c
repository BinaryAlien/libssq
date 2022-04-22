#include <stdlib.h>
#include "ssq/packet.h"
#include "ssq/query.h"

#ifndef _WIN32
# include <unistd.h>
# define INVALID_SOCKET (-1)
# define SOCKET_ERROR   (-1)
# define closesocket    close
typedef int SOCKET;
#endif /* _WIN32 */

#define SSQ_PACKET_SIZE 1400

static SOCKET ssq_query_init_socket(SSQ_QUERIER *const querier) {
    SOCKET sockfd = INVALID_SOCKET;

    for (struct addrinfo *addr = querier->addr_list; addr != NULL; addr = addr->ai_next) {
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
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)(&(querier->timeout_recv)), sizeof (querier->timeout_recv)) == SOCKET_ERROR ||
            setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&(querier->timeout_send)), sizeof (querier->timeout_send)) == SOCKET_ERROR) {
#ifdef _WIN32
            ssq_error_set_from_wsa(&(querier->err));
#else /* not _WIN32 */
            ssq_error_set_from_errno(&(querier->err));
#endif /* _WIN32 */
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
        }
    } else {
        ssq_error_set(&(querier->err), SSQ_ERR_NOENDPOINT, "No endpoints available to communicate with the target server");
    }

    return sockfd;
}

static void ssq_query_send(const SOCKET sockfd, const uint8_t payload[], const size_t payload_len, SSQ_ERROR *const err) {
#ifdef _WIN32
    if (send(sockfd, payload, (int)payload_len, 0) == SOCKET_ERROR)
        ssq_error_set_from_wsa(err);
#else /* not _WIN32 */
    if (send(sockfd, payload, payload_len, 0) == SOCKET_ERROR)
        ssq_error_set_from_errno(err);
#endif /* _WIN32 */
}

static SSQ_PACKET **ssq_query_recv(const SOCKET sockfd, uint8_t *const packet_count, SSQ_ERROR *const err) {
    *packet_count = 1;

    SSQ_PACKET **packets = NULL;

    for (uint8_t packets_received = 0; packets_received < *packet_count; ++packets_received) {
        uint8_t datagram[SSQ_PACKET_SIZE];

#ifdef _WIN32
        const int bytes_received = recv(sockfd, (char *)datagram, SSQ_PACKET_SIZE, 0);
#else /* not _WIN32 */
        const ssize_t bytes_received = recv(sockfd, datagram, SSQ_PACKET_SIZE, 0);
#endif /* not _WIN32 */

        if (bytes_received != SOCKET_ERROR) {
            SSQ_PACKET *const packet = ssq_packet_from_datagram(datagram, bytes_received, err);
            if (err->code != SSQ_OK) break;

            const bool is_first_packet = (packets == NULL);
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
#else /* not _WIN32 */
            ssq_error_set_from_errno(err);
#endif /* _WIN32 */
            break;
        }
    }

    if (err->code != SSQ_OK && packets != NULL) {
        ssq_packets_free(packets, *packet_count);
        packets = NULL;
    }

    return packets;
}

uint8_t *ssq_query(
    SSQ_QUERIER *const querier,
    const uint8_t      payload[],
    const size_t       payload_len,
    size_t      *const response_len
) {
    uint8_t *response = NULL;

    const SOCKET sockfd = ssq_query_init_socket(querier);
    if (!ssq_ok(querier)) return NULL;

    ssq_query_send(sockfd, payload, payload_len, &(querier->err));
    if (!ssq_ok(querier)) goto end;

    uint8_t            packet_count = 0;
    SSQ_PACKET **const packets      = ssq_query_recv(sockfd, &packet_count, &(querier->err));
    if (!ssq_ok(querier)) goto end;

    const SSQ_PACKET *const *const packets_readonly = (const SSQ_PACKET *const *)packets;

    if (ssq_packets_verify_integrity(packets_readonly, packet_count))
        response = ssq_packets_to_response(packets_readonly, packet_count, response_len, &(querier->err));
    else
        ssq_error_set(&(querier->err), SSQ_ERR_BADRES, "Packet IDs mismatch");

    ssq_packets_free(packets, packet_count);

end:
    closesocket(sockfd);

    return response;
}
