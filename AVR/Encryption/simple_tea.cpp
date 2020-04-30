#include "simple_tea.h"

constexpr uint8_t kKeyMaxIndex{kKeySize - 1};

void encrypt(uint8_t num_rounds, uint8_t v[2], uint8_t const key[16])
{
    uint8_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x39;
    for (uint8_t i = 0; i < num_rounds; i++)
    {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & kKeyMaxIndex]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 3) & kKeyMaxIndex]);
    }
    v[0] = v0;
    v[1] = v1;
}

void decrypt(uint8_t num_rounds, uint8_t v[2], uint8_t const key[16])
{
    uint8_t v0 = v[0], v1 = v[1], delta = 0x39, sum = delta * num_rounds;
    for (uint8_t i = 0; i < num_rounds; i++)
    {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 3) & kKeyMaxIndex]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & kKeyMaxIndex]);
    }
    v[0] = v0;
    v[1] = v1;
}
