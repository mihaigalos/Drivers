#pragma once

#include <stdint.h>

constexpr uint8_t kKeySize{16};

class SimpleTEA
{
public:
    void encrypt(uint8_t num_rounds, uint8_t const key[16], uint8_t payload[], uint8_t half_payload_length);
    void decrypt(uint8_t num_rounds, uint8_t const key[16], uint8_t payload[], uint8_t half_payload_length);

private:
    uint8_t delta_{0x39};
};
