#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "simple_tea_v5.h"
#include "simple_tea_v6.h"

constexpr uint8_t kPayloadSize{4};

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

using MyTypes = ::testing::Types<
    SimpleTEA_v5<kPayloadSize>,
    SimpleTEA_v6<kPayloadSize>>;

TYPED_TEST_SUITE(Fixture, MyTypes);

TYPED_TEST(Fixture, EncryptDecryptFourBytesWorks_WhenTypical)
{
    for (uint32_t i = 0x100000; i < 0x1000000; ++i)
    {
        auto expected = i;
        uint8_t v[kPayloadSize] = {static_cast<uint8_t>(i), static_cast<uint8_t>(i >> 8), static_cast<uint8_t>(i >> 16), static_cast<uint8_t>(i >> 24)};
        uint8_t crc = static_cast<uint8_t>(i);
        this->sut_.encrypt(v, crc);
        this->sut_.decrypt(v, crc);

        auto actual = static_cast<uint32_t>(v[0]) | (static_cast<uint32_t>(v[1]) << 8) | (static_cast<uint32_t>(v[2]) << 16) | (static_cast<uint32_t>(v[3]) << 24);
        ASSERT_EQ(expected, actual);
    }
}
