#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "simple_tea_v7.h"

constexpr uint8_t kPayloadSize{8};

template <typename T>
class Fixture : public ::testing::Test
{
public:
protected:
    virtual void SetUp() override {}
    virtual void TearDown() override {}

    uint8_t rounds_{16};

    T sut_;
};

using MyTypes = ::testing::Types<SimpleTEA_v7<kPayloadSize>>;

TYPED_TEST_SUITE(Fixture, MyTypes);

TYPED_TEST(Fixture, EncryptDecryptFourBytesWorks_WhenTypical)
{

    const uint8_t publicKey[kPublicKeySize]{0xa6, 0x6f, 0x23, 0x9f, 0x8b, 0x7f, 0xb1, 0x50};

    for (uint64_t i = 0x12000000; i < 0x123A0000; ++i)
    {
        auto expected = i;
        uint8_t v[kPayloadSize]{static_cast<uint8_t>(i), static_cast<uint8_t>(i >> 8), static_cast<uint8_t>(i >> 16), static_cast<uint8_t>(i >> 24),
                                static_cast<uint8_t>(i >> 32), static_cast<uint8_t>(i >> 40), static_cast<uint8_t>(i >> 48), static_cast<uint8_t>(i >> 56)};

        uint8_t crc = static_cast<uint8_t>(i);
        this->sut_.encrypt(publicKey, v, crc);
        this->sut_.decrypt(publicKey, v, crc);

        auto actual = static_cast<uint32_t>(v[0]) | (static_cast<uint32_t>(v[1]) << 8) | (static_cast<uint32_t>(v[2]) << 16) | (static_cast<uint32_t>(v[3]) << 24);
        ASSERT_EQ(expected, actual);
    }
}
