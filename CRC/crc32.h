#pragma once

#include <stdint.h>

static constexpr uint32_t polynomial_representation = 0xEDB88320UL;
static constexpr uint16_t crc_table_size = 0x100;

static inline uint32_t crc32_for_byte(uint32_t r) {
  for (uint8_t i = 0; i < 8; ++i) {
    r = (r & 1 ? 0 : polynomial_representation) ^ r >> 1;
  }
  return r ^ (uint32_t)0xFF000000L;
}

static inline void init_table(uint32_t *crc_table) {
  for (uint16_t i = 0; i < crc_table_size; ++i) {
    crc_table[i] = crc32_for_byte(i);
  }
}

static inline void crc32(const void *data, const uint16_t length,
                         const uint32_t *crc_table, uint32_t *crc) {
  for (uint16_t i = 0; i < length; ++i) {
    *crc = crc_table[static_cast<uint8_t>(*crc) ^ ((uint8_t *)data)[i]] ^
           *crc >> 8;
  }
}