// This #include statement was automatically added by the Particle IDE.
#include "e2prom.h"

E2PROM *e = NULL;

#define OWNADDRESS 0x50 // 127 maximum devices, 0b0101000 << 1 + 1 bit R/W (automatically added by Wire) = 0x50

void setup() {
    e = new E2PROM(OWNADDRESS); 
    delay(1000);
    
    uint8_t buffer[] = {"Hello World!"};
    e->writePage(0, &buffer[0], 8);
}

void loop() {
    e->dump();
    
    delay(2000);
}