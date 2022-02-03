#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "simple_tea.h"
#include "simple_tea_v2.h"
#include "simple_tea_v3.h"
#include "simple_tea_v4.h"

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

uint8_t key[kKeySize] = {0x45, 0x74, 0x32, 0x11, 0x98, 0x94, 0xAB, 0xCF, 0x90, 0xAE, 0xBA, 0xDC, 0x06, 0x16, 0x81, 0x95};

using MyTypes = ::testing::Types<SimpleTEA<kPayloadSize>,
                                 SimpleTEA_v2<kPayloadSize>,
                                 SimpleTEA_v3<kPayloadSize>,
                                 SimpleTEA_v4<kPayloadSize>>;

TYPED_TEST_SUITE(Fixture, MyTypes);

TYPED_TEST(Fixture, EncryptDecryptEightBytesWorks_WhenTypical)
{
    for (uint64_t i = 0x1234FFFCBA000000; i < 0x1234FFDCBA000000; ++i)
    {
        auto expected = i;
        uint8_t v[kPayloadSize]{static_cast<uint8_t>(i), static_cast<uint8_t>(i >> 8), static_cast<uint8_t>(i >> 16), static_cast<uint8_t>(i >> 24),
                                static_cast<uint8_t>(i >> 32), static_cast<uint8_t>(i >> 40), static_cast<uint8_t>(i >> 48), static_cast<uint8_t>(i >> 56)};

        this->sut_.encrypt(this->rounds_, key, v);
        this->sut_.decrypt(this->rounds_, key, v);

        uint64_t actual = static_cast<uint64_t>(v[0]) | (static_cast<uint64_t>(v[1]) << 8) | (static_cast<uint64_t>(v[2]) << 16) | (static_cast<uint64_t>(v[3]) << 24) |
                          (static_cast<uint64_t>(v[4]) << 32) | (static_cast<uint64_t>(v[5]) << 40) | (static_cast<uint64_t>(v[6]) << 48) | (static_cast<uint64_t>(v[7]) << 56);
        ASSERT_EQ(expected, actual);
    }
}

TYPED_TEST(Fixture, EncryptDecryptEightBytesWorks_WhenRandomData)
{
    auto expected = 0xABCDEF9678ADECAB;
    uint8_t v[kPayloadSize]{static_cast<uint8_t>(expected), static_cast<uint8_t>(expected >> 8), static_cast<uint8_t>(expected >> 16), static_cast<uint8_t>(expected >> 24),
                            static_cast<uint8_t>(expected >> 32), static_cast<uint8_t>(expected >> 40), static_cast<uint8_t>(expected >> 48), static_cast<uint8_t>(expected >> 56)};

    this->sut_.encrypt(this->rounds_, key, v);
    this->sut_.decrypt(this->rounds_, key, v);

    uint64_t actual = static_cast<uint64_t>(v[0]) | (static_cast<uint64_t>(v[1]) << 8) | (static_cast<uint64_t>(v[2]) << 16) | (static_cast<uint64_t>(v[3]) << 24) |
                        (static_cast<uint64_t>(v[4]) << 32) | (static_cast<uint64_t>(v[5]) << 40) | (static_cast<uint64_t>(v[6]) << 48) | (static_cast<uint64_t>(v[7]) << 56);
    ASSERT_EQ(expected, actual);
}