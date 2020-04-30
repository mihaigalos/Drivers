#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "simple_tea2.h"

constexpr uint8_t kHalfPayloadSize{4};

class Fixture : public ::testing::Test
{
public:
protected:
    virtual void SetUp() override {}
    virtual void TearDown() override {}

    uint8_t rounds_{16};
    static uint8_t key[kKeySize];

    SimpleTEA<kHalfPayloadSize> sut_;
};

uint8_t Fixture::key[kKeySize] = {0x45, 0x74, 0x32, 0x11, 0x98, 0x94, 0xAB, 0xCF, 0x90, 0xAE, 0xBA, 0xDC, 0x06, 0x16, 0x81, 0x95};

TEST_F(Fixture, EncryptDecryptEightBytesWorks_WhenTypical)
{
    for (uint64_t i = 0x1234FFFCBA000000; i < 0x1234FFDCBA000000; ++i)
    {
        rounds_ = 2;
        auto expected = i;
        uint8_t v[kHalfPayloadSize * 2]{static_cast<uint8_t>(i), static_cast<uint8_t>(i >> 8), static_cast<uint8_t>(i >> 16), static_cast<uint8_t>(i >> 24),
                                        static_cast<uint8_t>(i >> 32), static_cast<uint8_t>(i >> 40), static_cast<uint8_t>(i >> 48), static_cast<uint8_t>(i >> 56)};

        sut_.encrypt(rounds_, key, v);
        sut_.decrypt(rounds_, key, v);
        uint64_t actual = static_cast<uint64_t>(v[0]) | (static_cast<uint64_t>(v[1]) << 8) | (static_cast<uint64_t>(v[2]) << 16) | (static_cast<uint64_t>(v[3]) << 24) |
                          (static_cast<uint64_t>(v[4]) << 32) | (static_cast<uint64_t>(v[5]) << 40) | (static_cast<uint64_t>(v[6]) << 48) | (static_cast<uint64_t>(v[7]) << 56);

        ASSERT_EQ(expected, actual);
    }
}