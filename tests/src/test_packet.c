#include <criterion/criterion.h>
#include "ssq/packet.h"

static size_t _memcpy(
    void       *const dst,
    const void *const src,
    const size_t      n
) {
    memcpy(dst, src, n);
    return n;
}

static SSQ_PACKET **helper_init_empty(const uint8_t packet_count) {
    SSQ_PACKET **const packets = calloc(packet_count, sizeof (SSQ_PACKET *));

    for (uint8_t i = 0; i < packet_count; ++i) {
        packets[i] = malloc(sizeof (SSQ_PACKET));
        memset(packets[i], 0, sizeof (SSQ_PACKET));
    }

    return packets;
}

static size_t helper_serialize_single(const SSQ_PACKET *const packet, char buf[]) {
    size_t len = 0;

    len += _memcpy(buf + len, &(packet->header), sizeof (packet->header));
    len += _memcpy(buf + len, packet->payload,   packet->payload_len);

    return len;
}

static size_t helper_serialize_multi(const SSQ_PACKET *const packet, char buf[]) {
    size_t len = 0;

    len += _memcpy(buf + len, &(packet->header), sizeof (packet->header));
    len += _memcpy(buf + len, &(packet->id),     sizeof (packet->id));
    len += _memcpy(buf + len, &(packet->total),  sizeof (packet->total));
    len += _memcpy(buf + len, &(packet->number), sizeof (packet->number));
    len += _memcpy(buf + len, &(packet->size),   sizeof (packet->size));
    len += _memcpy(buf + len, packet->payload,   packet->payload_len);

    return len;
}

Test(packet, init_single, .description="Single-response packet deserialization") {
    const SSQ_PACKET ref = {
        .header      = A2S_PACKET_HEADER_SINGLE,
        .id          = 0,
        .total       = 1,
        .number      = 0,
        .size        = 0,
        .payload     = "somedata",
        .payload_len = 9
    };

    char         datagram[64];
    const size_t datagram_len = helper_serialize_single(&ref, datagram);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *const packet = ssq_packet_init(datagram, datagram_len, &err);

    cr_assert(err.code == SSQ_OK);

    cr_assert(packet              != NULL);
    cr_assert(packet->header      == ref.header);
    cr_assert(packet->id          == ref.id);
    cr_assert(packet->total       == ref.total);
    cr_assert(packet->number      == ref.number);
    cr_assert(packet->size        == ref.size);
    cr_assert(packet->payload     != NULL);
    cr_assert(packet->payload_len == ref.payload_len);
    cr_assert_arr_eq(packet->payload, ref.payload, ref.payload_len);

    ssq_packet_free(packet);
}

Test(packet, init_multi, .description="Multi-response packet deserialization") {
    const SSQ_PACKET ref = {
        .header      = A2S_PACKET_HEADER_MULTI,
        .id          = 0x12345678,
        .total       = 3,
        .number      = 2,
        .size        = 1260,
        .payload     = "somedata",
        .payload_len = 9
    };

    char         datagram[128];
    const size_t datagram_len = helper_serialize_multi(&ref, datagram);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *const packet = ssq_packet_init(datagram, datagram_len, &err);

    cr_assert(err.code == SSQ_OK);

    cr_assert(packet              != NULL);
    cr_assert(packet->header      == ref.header);
    cr_assert(packet->id          == ref.id);
    cr_assert(packet->total       == ref.total);
    cr_assert(packet->number      == ref.number);
    cr_assert(packet->size        == ref.size);
    cr_assert(packet->payload     != NULL);
    cr_assert(packet->payload_len == ref.payload_len);
    cr_assert_arr_eq(packet->payload, ref.payload, ref.payload_len);

    ssq_packet_free(packet);
}

Test(packet, init_compressed, .description="Multi-response compressed packet deserialization") {
    const SSQ_PACKET ref = {
        .header      = A2S_PACKET_HEADER_MULTI,
        .id          = 0x12345678 | A2S_PACKET_ID_COMPRESSION_FLAG,
        .total       = 3,
        .number      = 0,
        .size        = 1260,
        .payload     = "somedata",
        .payload_len = 9
    };

    char         datagram[128];
    const size_t datagram_len = helper_serialize_multi(&ref, datagram);

    SSQ_ERROR err;
    ssq_error_clear(&err);

    SSQ_PACKET *const packet = ssq_packet_init(datagram, datagram_len, &err);

    cr_assert(err.code == SSQ_ERR_UNSUPPORTED);
    cr_assert(packet   == NULL);
}

Test(packet, idcheck, .description="Packets ID check") {
    const uint8_t       packet_count = 4;
    SSQ_PACKET  **const packets      = helper_init_empty(packet_count);

    for (uint8_t i = 0; i < packet_count; ++i)
        packets[i]->id = 0x12345678;

    const SSQ_PACKET *const *const packets_readonly = (const SSQ_PACKET *const *)packets;

    cr_assert(ssq_packet_idcheck(packets_readonly, packet_count));

    // invalidate one packet's ID
    packets[0]->id = 0x00000000;

    cr_assert(!ssq_packet_idcheck(packets_readonly, packet_count));

    ssq_packet_freemany(packets, packet_count);
}

Test(packet, concatpayloads, .description="Packets payload concatenation") {
    const size_t  payload_fragment_len = 3;
    const uint8_t packet_count         = 3;
    const size_t  payload_len          = payload_fragment_len * packet_count;

    const char   payload[] = {
        0x12, 0x34, 0x56, // Fragment #0
        0x23, 0x45, 0x67, // Fragment #1
        0x34, 0x56, 0x78  // Fragment #2
    };

    SSQ_PACKET **const packets = helper_init_empty(packet_count);

    // Load the payload fragments in their respective packet
    for (uint8_t i = 0; i < packet_count; ++i) {
        const char *const payload_fragment = payload + i * payload_fragment_len;

        packets[i]->payload_len = payload_fragment_len;
        packets[i]->payload     = malloc(packets[i]->payload_len);

        memcpy(packets[i]->payload, payload_fragment, payload_fragment_len);
    }

    SSQ_ERROR err;
    ssq_error_clear(&err);

    size_t       out_len = 0;
    char  *const out     = ssq_packet_concatpayloads((const SSQ_PACKET *const *)packets, packet_count, &out_len, &err);

    cr_assert(err.code == SSQ_OK);

    cr_assert(out      != NULL);
    cr_assert(out_len  == payload_len);
    cr_assert_arr_eq(out, payload, payload_len);

    free(out);
    ssq_packet_freemany(packets, packet_count);
}
