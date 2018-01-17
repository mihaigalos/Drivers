#pragma once

#include <stdint.h>

static constexpr uint32_t polynomial_representation = 0xEDB88320;

static inline uint32_t crc32 (uint32_t crc, uint8_t *buffer, int length=2) { 
    int i, j; 
    for (i=0; i<length; i++) { 
        crc = crc ^ *(buffer++); 
        for (j=0; j<8; j++) { 
		    if (crc & 1) 
		        crc = (crc>>1) ^ polynomial_representation ; 
		    else 
		        crc = crc >>1 ; 
        } 
    } 
    return crc; 
}