#include <fstream>
#include <gtest/gtest.h>

#include <ssq/packet.h>

static constexpr size_t SSQ_PACKET_HEADER_LEN = sizeof (int32_t);

static size_t read_datagram_len(const std::string &filename) {
    auto stream = std::ifstream{filename, std::ifstream::binary | std::ifstream::ate};
    if (!stream)
        return 0;
    return stream.tellg();
}

static uint8_t *read_datagram(const std::string &filename, size_t &datagram_len) {
    auto stream = std::ifstream{filename, std::ifstream::binary};
    if (!stream)
        return nullptr;
    datagram_len = read_datagram_len(filename);
    auto datagram = new char[datagram_len];
    if (!stream.read(datagram, datagram_len)) {
        delete[] datagram;
        return nullptr;
    }
    return reinterpret_cast<uint8_t*>(datagram);
}

TEST(packet, from_datagram_single) {
    size_t datagram_len;
    uint8_t *datagram = read_datagram("bin/info/cstrike.bin", datagram_len);
    if (!datagram)
        FAIL() << "Could not read the datagram. Is `tests/bin' in the current working directory?";

    SSQ_ERROR error;
    ssq_error_clear(&error);
    SSQ_PACKET *packet = ssq_packet_from_datagram(datagram, datagram_len, &error);
    EXPECT_EQ(error.code, SSQE_OK);
    ASSERT_TRUE(packet);
    EXPECT_EQ(packet->header, SSQ_PACKET_HEADER_SINGLE);
    EXPECT_EQ(packet->total, 1);
    EXPECT_EQ(packet->number, 0);
    EXPECT_EQ(packet->payload_len, datagram_len - SSQ_PACKET_HEADER_LEN);
    EXPECT_STREQ(reinterpret_cast<char *>(packet->payload), reinterpret_cast<char *>(datagram + SSQ_PACKET_HEADER_LEN));

    ssq_packet_free(packet);
    delete[] datagram;
}
