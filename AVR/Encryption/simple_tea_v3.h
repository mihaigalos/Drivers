#pragma once

#include <stdint.h>

#include "common.h"

template <uint8_t PayloadSize, uint8_t Delta = 0x39>
class SimpleTEA_v3
{
public:
    void encrypt(uint8_t num_rounds, uint8_t const (&key)[kKeySize], uint8_t payload[])
    {
        for (uint8_t i = 2; i <= PayloadSize; i = i + 2)
        {
            uint8_t p0 = payload[i - 2], p1 = payload[i - 1], sum = 0;
            for (uint8_t j = 0; j < num_rounds; ++j)
            {
                p0 += ((p1 << 4) ^ (p1 >> 3)) ^ (key[sum & kPrivateKeyMaxIndex]);
                sum += Delta;
                p1 += ((p0 << 4) ^ (p0 >> 3)) ^ (key[kPrivateKeyMaxIndex]);
            }
            payload[i - 2] = p0;
            payload[i - 1] = p1;
        }
    }

    void decrypt(uint8_t num_rounds, uint8_t const (&key)[kKeySize], uint8_t payload[])
    {
        for (uint8_t i = 2; i <= PayloadSize; i = i + 2)
        {
            uint8_t p0 = payload[i - 2], p1 = payload[i - 1], sum = Delta * num_rounds;
            for (uint8_t i = 0; i < num_rounds; ++i)
            {
                p1 -= ((p0 << 4) ^ (p0 >> 3)) ^ (key[kPrivateKeyMaxIndex]);
                sum -= Delta;
                p0 -= ((p1 << 4) ^ (p1 >> 3)) ^ (key[sum & kPrivateKeyMaxIndex]);
            }
            payload[i - 2] = p0;
            payload[i - 1] = p1;
        }
    }
};