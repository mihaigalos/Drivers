#pragma once

#include <stdint.h>

constexpr uint8_t kKeySize{16};

void encrypt(uint8_t num_rounds, uint8_t v[2], uint8_t const key[16]);
void decrypt(uint8_t num_rounds, uint8_t v[2], uint8_t const key[16]);