
#include <avr/io.h>
#include "SoftwareUart.h"

void setup()
{
  pinMode(1, OUTPUT); //LED
  digitalWrite(1, LOW);

  uart_init();
}

void loop()
{
  for (uint8_t c = 'a'; c < 'z'; ++c) {
    uart_write(c);
  }
  delay(10);
}
