#pragma once

#include <stdint.h>

#include "common.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif //__AVR__

constexpr uint8_t kNumberOfKeys{6};
constexpr uint8_t kNumberOfKeysMaxIndex{kNumberOfKeys - 1};

constexpr uint8_t rounds PROGMEM{16};
constexpr uint8_t encription_key[kNumberOfKeys][kKeySize] PROGMEM = {
    {0x45, 0x74, 0x32, 0x11, 0x98, 0x94, 0xAB, 0xCF, 0x90, 0xAE, 0xBA, 0xDC, 0x06, 0x16, 0x81, 0x95},
    {0x67, 0xc2, 0xce, 0x7d, 0xad, 0x54, 0x41, 0x8a, 0xc4, 0x89, 0xd9, 0x4f, 0x02, 0x9f, 0x6e, 0x24},
    {0x05, 0x69, 0x95, 0xc4, 0xd9, 0x9c, 0x96, 0x1a, 0x95, 0x51, 0xc8, 0xa5, 0x35, 0x5f, 0xdb, 0x9a},
    {0xa2, 0x39, 0x63, 0xd2, 0xf0, 0x48, 0x8d, 0xf2, 0xce, 0x65, 0xcb, 0x25, 0xca, 0x98, 0x43, 0xce},
    {0x82, 0xbb, 0xc3, 0xdd, 0x2f, 0xb9, 0x46, 0x95, 0xc9, 0x7f, 0x73, 0x98, 0x47, 0xff, 0x07, 0x2c},
    {0xd0, 0x54, 0xc3, 0x61, 0x8e, 0x01, 0x1a, 0x67, 0x52, 0x0c, 0xde, 0x1d, 0x34, 0xa5, 0x08, 0x84},
};

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
                p0 += ((p1 << 4) ^ (p1 >> 3)) ^ (encription_key[crc & kNumberOfKeysMaxIndex][sum & kKeyMaxIndex]);
                sum += Delta;
                p1 += ((p0 << 4) ^ (p0 >> 3)) ^ (encription_key[crc & kNumberOfKeysMaxIndex][kKeyMaxIndex]);
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
                p1 -= ((p0 << 4) ^ (p0 >> 3)) ^ (encription_key[crc & kNumberOfKeysMaxIndex][kKeyMaxIndex]);
                sum -= Delta;
                p0 -= ((p1 << 4) ^ (p1 >> 3)) ^ (encription_key[crc & kNumberOfKeysMaxIndex][sum & kKeyMaxIndex]);
            }
            payload[i - 2] = p0;
            payload[i - 1] = p1;
        }
    }

private:
};
