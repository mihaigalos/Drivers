#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "simple_tea.h"

constexpr uint8_t kHalfPayloadSize{1};

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

TEST_F(Fixture, EncryptTookPlace_WhenTypical)
{
    uint8_t allowed_identical_raw_vs_encrypted_count{10};
    uint16_t identical{};
    for (uint16_t i = 0; i < 0xFFFF; ++i)
    {
        uint8_t v[kHalfPayloadSize * 2] = {static_cast<uint8_t>(i), static_cast<uint8_t>(i >> 8)};

        sut_.encrypt(rounds_, key, v);
        auto actual = static_cast<uint16_t>(v[0]) | (static_cast<uint16_t>(v[1]) << 8);

        if (actual == i)
        {
            ++identical;
        }
    }

    ASSERT_LT(identical, allowed_identical_raw_vs_encrypted_count);
}

TEST_F(Fixture, EncryptDecryptTwoBytesWorks_WhenTypical)
{
    for (uint16_t i = 0; i < 0xFFFF; ++i)
    {
        auto expected = i;
        uint8_t v[kHalfPayloadSize * 2] = {static_cast<uint8_t>(i), static_cast<uint8_t>(i >> 8)};

        sut_.encrypt(rounds_, key, v);
        sut_.decrypt(rounds_, key, v);

        auto actual = static_cast<uint16_t>(v[0]) | (static_cast<uint16_t>(v[1]) << 8);
        ASSERT_EQ(expected, actual);
    }
}

TEST_F(Fixture, EncryptDecryptTwoBytesFails_WhenImproperContent)
{

    for (uint16_t i = 0; i < 0xFFFF; ++i)
    {
        auto expected = i;
        uint8_t v[kHalfPayloadSize * 2] = {static_cast<uint8_t>(i), static_cast<uint8_t>(i >> 8)};

        sut_.encrypt(rounds_, key, v);
        sut_.decrypt(rounds_, key, v);
        v[0] = v[0] + 1;

        auto actual = static_cast<uint16_t>(v[0]) | (static_cast<uint16_t>(v[1]) << 8);
        ASSERT_NE(expected, actual);
    }
}
