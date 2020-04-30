#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "simple_tea.h"

class Fixture : public ::testing::Test
{
public:
protected:
    virtual void SetUp() override {}
    virtual void TearDown() override {}

    uint8_t rounds{16};
    static uint8_t key[kKeySize];
};

uint8_t Fixture::key[kKeySize] = {0x45, 0x74, 0x32, 0x11, 0x98, 0x94, 0xAB, 0xCF, 0x90, 0xAE, 0xBA, 0xDC, 0x06, 0x16, 0x81, 0x95};

TEST_F(Fixture, EncryptDecryptWorks_WhenTypical)
{
    for (uint16_t i = 0; i < 0xFFFF; ++i)
    {
        auto expected = i;
        uint8_t v[] = {static_cast<uint8_t>(i), static_cast<uint8_t>(i >> 8)};
        encrypt(rounds, v, key);
        decrypt(rounds, v, key);
        auto actual = static_cast<uint16_t>(v[0]) | (static_cast<uint16_t>(v[1]) << 8);

        ASSERT_EQ(expected, actual);
    }
}

TEST_F(Fixture, EncryptDecryptFails_WhenImproperContent)
{

    for (uint16_t i = 0; i < 0xFFFF; ++i)
    {
        auto expected = i;
        uint8_t v[] = {static_cast<uint8_t>(i), static_cast<uint8_t>(i >> 8)};
        encrypt(rounds, v, key);
        decrypt(rounds, v, key);
        v[0] = v[0] + 1;
        auto actual = static_cast<uint16_t>(v[0]) | (static_cast<uint16_t>(v[1]) << 8);

        ASSERT_NE(expected, actual);
    }
}