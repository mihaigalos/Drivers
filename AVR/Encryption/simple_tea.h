#pragma once

#include <stdint.h>

constexpr uint8_t kKeySize{16};
constexpr uint8_t kKeyMaxIndex{kKeySize - 1};

template <uint8_t HalfPayloadSize, uint8_t Delta = 0x39>
class SimpleTEA
{
public:
    void encrypt(uint8_t num_rounds, uint8_t const key[kKeySize], uint8_t payload[])
    {
        for (uint8_t i = 2; i <= HalfPayloadSize * 2; i = i + 2)
        {
            uint8_t p0 = payload[i - 2], p1 = payload[i - 1], sum = 0;
            for (uint8_t j = 0; j < num_rounds; ++j)
            {
                p0 += (((p1 << 4) ^ (p1 >> 5)) + p1) ^ (sum + key[sum & kKeyMaxIndex]);
                sum += Delta;
                p1 += (((p0 << 4) ^ (p0 >> 5)) + p0) ^ (sum + key[(sum >> 3) & kKeyMaxIndex]);
            }
            payload[i - 2] = p0;
            payload[i - 1] = p1;
        }
    }

    void decrypt(uint8_t num_rounds, uint8_t const key[kKeySize], uint8_t payload[])
    {
        for (uint8_t i = 2; i <= HalfPayloadSize * 2; i = i + 2)
        {
            uint8_t p0 = payload[i - 2], p1 = payload[i - 1], sum = Delta * num_rounds;
            for (uint8_t i = 0; i < num_rounds; i++)
            {
                p1 -= (((p0 << 4) ^ (p0 >> 5)) + p0) ^ (sum + key[(sum >> 3) & kKeyMaxIndex]);
                sum -= Delta;
                p0 -= (((p1 << 4) ^ (p1 >> 5)) + p1) ^ (sum + key[sum & kKeyMaxIndex]);
            }
            payload[i - 2] = p0;
            payload[i - 1] = p1;
        }
    }
};
