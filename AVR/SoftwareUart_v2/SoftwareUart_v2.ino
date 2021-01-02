
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
  uart_write(uart_read());
}
