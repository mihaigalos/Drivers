#pragma once

#include <stdint.h>

typedef enum {
    TEEepromResult_Ok,
    TEEepromResult_BufferGreaterAsPageSize
}TEEepromResult;

class E2PROM{
    static uint8_t ownAddress_;
    
    public: 
    
    E2PROM(uint8_t ownAddress, uint32_t speed=CLOCK_SPEED_400KHZ);
    
    uint8_t readByte(uint16_t registerAddress);
    void writeByte(uint16_t registerAddress, uint8_t data);
    TEEepromResult  writePage(uint16_t registerAddress, uint8_t* buffer, uint8_t byteCount);
    TEEepromResult  readPage (uint16_t registerAddress, uint8_t* buffer, uint8_t byteCount);
    void dump(uint8_t columnCount = 8);
};