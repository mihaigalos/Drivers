#pragma once

#include <stdint.h>

#include "common.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif //__AVR__

constexpr uint8_t rounds PROGMEM{16};
constexpr uint8_t encription_key[kKeySize] PROGMEM = {0x45, 0x74, 0x32, 0x11, 0x98, 0x94, 0xAB, 0xCF, 0x90, 0xAE, 0xBA, 0xDC, 0x06, 0x16, 0x81, 0x95};

template <uint8_t PayloadSize, uint8_t Delta = 0x39>
class SimpleTEA_v4
{
public:
    void encrypt(uint8_t num_rounds, uint8_t const (&key)[kKeySize], uint8_t payload[])
    {
        (void)num_rounds;
        (void)key;
        encrypt(payload);
    }
    void encrypt(uint8_t payload[])
    {
        uint8_t sum = 0;
        for (uint8_t i = 2; i <= PayloadSize; i = i + 2)
        {
            uint8_t p0 = payload[i - 2], p1 = payload[i - 1];
            for (uint8_t j = 0; j < rounds; ++j)
            {
                p0 += ((p1 << 4) ^ (p1 >> 3)) ^ (encription_key[sum & kKeyMaxIndex]);
                sum += Delta;
                p1 += ((p0 << 4) ^ (p0 >> 3)) ^ (encription_key[kKeyMaxIndex]);
            }
            payload[i - 2] = p0;
            payload[i - 1] = p1;
        }
    }

    void decrypt(uint8_t num_rounds, uint8_t const (&key)[kKeySize], uint8_t payload[])
    {
        (void)num_rounds;
        (void)key;
        decrypt(payload);
    }
    void decrypt(uint8_t payload[])
    {
        uint8_t sum = static_cast<uint8_t>(PayloadSize * Delta * rounds);
        for (uint8_t i = 2; i <= PayloadSize; i = i + 2)
        {
            uint8_t p0 = payload[i - 2], p1 = payload[i - 1];
            for (uint8_t i = 0; i < rounds; ++i)
            {
                p1 -= ((p0 << 4) ^ (p0 >> 3)) ^ (encription_key[kKeyMaxIndex]);
                sum -= Delta;
                p0 -= ((p1 << 4) ^ (p1 >> 3)) ^ (encription_key[sum & kKeyMaxIndex]);
            }
            payload[i - 2] = p0;
            payload[i - 1] = p1;
        }
    }

private:
};