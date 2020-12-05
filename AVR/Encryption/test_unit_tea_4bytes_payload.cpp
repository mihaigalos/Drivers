#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "simple_tea.h"
#include "simple_tea_v2.h"
#include "simple_tea_v3.h"

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

using MyTypes = ::testing::Types<SimpleTEA<kPayloadSize>,
                                 SimpleTEA_v2<kPayloadSize>,
                                 SimpleTEA_v3<kPayloadSize>>;

uint8_t key[kKeySize] = {0x45, 0x74, 0x32, 0x11, 0x98, 0x94, 0xAB, 0xCF, 0x90, 0xAE, 0xBA, 0xDC, 0x06, 0x16, 0x81, 0x95};

TYPED_TEST_SUITE(Fixture, MyTypes);

TYPED_TEST(Fixture, EncryptDecryptFourBytesWorks_WhenTypical_Part1)
{
    for (uint32_t i = 0; i < 0x10000000; ++i)
    {
        auto expected = i;
        uint8_t v[kPayloadSize] = {static_cast<uint8_t>(i), static_cast<uint8_t>(i >> 8), static_cast<uint8_t>(i >> 16), static_cast<uint8_t>(i >> 24)};

        this->sut_.encrypt(this->rounds_, key, v);
        this->sut_.decrypt(this->rounds_, key, v);

        auto actual = static_cast<uint32_t>(v[0]) | (static_cast<uint32_t>(v[1]) << 8) | (static_cast<uint32_t>(v[2]) << 16) | (static_cast<uint32_t>(v[3]) << 24);
        ASSERT_EQ(expected, actual);
    }
}
