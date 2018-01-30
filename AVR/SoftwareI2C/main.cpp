#include "i2c.h"
#include <avr/io.h>

int main(void) {
  I2C i2c;
  i2c.Begin();

  while (1) {
    i2c.StartTransmission(0x42);
    i2c.Write(0x45);
    i2c.EndTransmission();

    _delay_ms(500);
  }
}
