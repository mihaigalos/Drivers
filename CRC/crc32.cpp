#include "crc32.h"

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void crc_on_buffer(uint8_t ac, char **av) {
  FILE *fp;
  char buf[1L << 15];
  uint32_t crc_table[crc_table_size];

  init_table(&crc_table[0]);
  for (uint8_t i = ac > 1; i < ac; ++i) {
    if ((fp = i ? fopen(av[i], "rb") : stdin)) {
      uint32_t crc = 0;
      while (!feof(fp) && !ferror(fp)) {
        crc32(buf, fread(buf, 1, sizeof(buf), fp), &crc_table[0], &crc);
      }
      std::cout << "CRC32: " << std::hex << crc << std::endl;

      if (i) {
        fclose(fp);
      }
    }
  }
}

void crc_on_word(uint8_t ac, char **av) {
  FILE *fp;
  uint8_t buf[2]; // 2 bytes : one word
  uint32_t crc_table[crc_table_size];

  init_table(&crc_table[0]);

  for (uint8_t i = ac > 1; i < ac; ++i) {
    if ((fp = i ? fopen(av[i], "rb") : stdin)) {
      uint32_t crc = 0;
      while (!feof(fp) && !ferror(fp)) {
        crc32(buf, fread(buf, 1, sizeof(buf), fp), &crc_table[0], &crc);
      }
      std::cout << "CRC32: " << std::hex << crc << std::endl;
    }
  }
}

int main(int ac, char **av) {
  crc_on_word(ac, av);
  return 0;
}