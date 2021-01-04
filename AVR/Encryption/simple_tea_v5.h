#pragma once

#include <stdint.h>

#include "common_key_select.h"

template <uint8_t PayloadSize, uint8_t Delta = 0x39>
class SimpleTEA_v5
{
public:
    void encrypt(uint8_t payload[], uint8_t crc)
    {
        uint8_t sum = 0;
        for (uint8_t i = 2; i <= PayloadSize; i = i + 2)
        {
            uint8_t p0 = payload[i - 2], p1 = payload[i - 1];
            for (uint8_t j = 0; j < rounds; ++j)
            {
                p0 += ((p1 << 4) ^ (p1 >> 3)) ^ (privateKey[crc & kNumberOfKeysMaxIndex][sum & kPrivateKeyMaxIndex]);
                sum += Delta;
                p1 += ((p0 << 4) ^ (p0 >> 3)) ^ (privateKey[crc & kNumberOfKeysMaxIndex][kPrivateKeyMaxIndex]);
            }
            payload[i - 2] = p0;
            payload[i - 1] = p1;
        }
    }

    void decrypt(uint8_t payload[], uint8_t crc)
    {
        uint8_t sum = static_cast<uint8_t>((PayloadSize / 2) * Delta * rounds);
        for (uint8_t i = 2; i <= PayloadSize; i = i + 2)
        {
            uint8_t p0 = payload[i - 2], p1 = payload[i - 1];
            for (uint8_t i = 0; i < rounds; ++i)
            {
                p1 -= ((p0 << 4) ^ (p0 >> 3)) ^ (privateKey[crc & kNumberOfKeysMaxIndex][kPrivateKeyMaxIndex]);
                sum -= Delta;
                p0 -= ((p1 << 4) ^ (p1 >> 3)) ^ (privateKey[crc & kNumberOfKeysMaxIndex][sum & kPrivateKeyMaxIndex]);
            }
            payload[i - 2] = p0;
            payload[i - 1] = p1;
        }
    }

private:
};
