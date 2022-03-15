#include <criterion/criterion.h>
#include "ssq/packet.h"

static size_t memcpy_with_offset(
    void       *const dst,
    const void *const src,
    const size_t      len,
    const size_t      offset
) {
    memcpy(dst + offset, src, len);
    return offset + len;
}

Test(packet, packet_init_single, .description="Single-response packet deserialization") {
    const int32_t header      = A2S_PACKET_HEADER_SINGLE;
    const char    payload[]   = "abc";
    const size_t  payload_len = sizeof (payload) / sizeof (*payload);

    char datagram[sizeof (header) + payload_len];
    const size_t datagram_len = sizeof (datagram) / sizeof (*datagram);

    memcpy(datagram, &header, sizeof (header));
    memcpy(datagram + sizeof (header), payload, payload_len);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *packet = ssq_packet_init(datagram, datagram_len, &err);

    cr_assert(err.code == SSQ_OK);

    cr_assert(packet != NULL);
    cr_assert(packet->header == header);
    cr_assert(packet->id == 0);
    cr_assert(packet->total == 1);
    cr_assert(packet->number == 0);
    cr_assert(packet->size == 0);
    cr_assert(packet->payload != NULL);
    cr_assert(packet->payload_len == payload_len);

    for (size_t i = 0; i < payload_len; ++i) {
        cr_assert(packet->payload[i] == payload[i]);
    }

    ssq_packet_free(packet);
}

Test(packet, packet_init_multi, .description="Multi-response packet deserialization") {
    const int32_t  header      = A2S_PACKET_HEADER_MULTI;
    const int32_t  id          = 0x12345678;
    const uint8_t  total       = 3;
    const uint8_t  number      = 1;
    const uint16_t size        = 1260;
    const char     payload[]   = "abc";
    const size_t   payload_len = sizeof (payload) / sizeof (*payload);

    char datagram[sizeof (header) + sizeof (id) + sizeof (total) + sizeof (number) + sizeof (size) + payload_len];
    const size_t datagram_len = sizeof (datagram) / sizeof (*datagram);

    size_t offset = 0;
    offset = memcpy_with_offset(datagram, &header, sizeof (header), offset);
    offset = memcpy_with_offset(datagram, &id, sizeof (id), offset);
    offset = memcpy_with_offset(datagram, &total, sizeof (total), offset);
    offset = memcpy_with_offset(datagram, &number, sizeof (number), offset);
    offset = memcpy_with_offset(datagram, &size, sizeof (size), offset);
    offset = memcpy_with_offset(datagram, payload, payload_len, offset);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *packet = ssq_packet_init(datagram, datagram_len, &err);

    cr_assert(err.code == SSQ_OK);

    cr_assert(packet != NULL);
    cr_assert(packet->header == header);
    cr_assert(packet->id == id);
    cr_assert(packet->total == total);
    cr_assert(packet->number == number);
    cr_assert(packet->size == size);
    cr_assert(packet->payload != NULL);
    cr_assert(packet->payload_len == payload_len);
    cr_assert_arr_eq(packet->payload, payload, payload_len);

    ssq_packet_free(packet);
}

Test(packet, packet_idcheck, .description="Packets ID check") {
    const uint8_t packet_count = 4;
    SSQ_PACKET  **packets      = calloc(packet_count, sizeof (SSQ_PACKET *));

    for (uint8_t i = 0; i < packet_count; ++i) {
        packets[i] = malloc(sizeof (SSQ_PACKET));
        memset(packets[i], 0, sizeof (SSQ_PACKET));
    }

    // set same ID for each packet
    for (uint8_t i = 0; i < packet_count; ++i)
        packets[i]->id = 0x12345678;

    const SSQ_PACKET *const *const packets_readonly = (const SSQ_PACKET *const *)packets;

    cr_assert(ssq_packet_idcheck(packets_readonly, packet_count));

    // invalidate one packet's ID
    packets[0]->id = 0x00000000;
    cr_assert(!ssq_packet_idcheck(packets_readonly, packet_count));

    ssq_packet_freemany(packets, packet_count);
}

Test(packet, packet_concatpayloads, .description="Packets payload concatenation") {
    const size_t  payload_fragment_len = 3;
    const uint8_t packet_count         = 3;
    const size_t  payload_len          = payload_fragment_len * packet_count;

    const char    payload[]            = "abc" "def" "ghi";

    SSQ_PACKET  **packets              = calloc(packet_count, sizeof (SSQ_PACKET *));

    for (uint8_t i = 0; i < packet_count; ++i) {
        packets[i] = malloc(sizeof (SSQ_PACKET));
        memset(packets[i], 0, sizeof (SSQ_PACKET));

        packets[i]->payload_len = payload_fragment_len;
        packets[i]->payload = malloc(packets[i]->payload_len);
        memcpy(packets[i]->payload, payload + payload_fragment_len * i, packets[i]->payload_len);
    }

    const SSQ_PACKET *const *const packets_readonly = (const SSQ_PACKET *const *)packets;

    SSQ_ERROR err;
    ssq_error_clear(&err);

    size_t out_len = 0;
    char  *out     = ssq_packet_concatpayloads(packets_readonly, packet_count, &out_len, &err);

    cr_assert(err.code == SSQ_OK);
    cr_assert(out != NULL);
    cr_assert(out_len == payload_len);
    cr_assert_arr_eq(out, payload, payload_len);

    free(out);
    ssq_packet_freemany(packets, packet_count);
}
