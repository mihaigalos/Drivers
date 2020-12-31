
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
  digitalWrite(1, HIGH);
  delay(1000 / 64); // wait for a second
  digitalWrite(1, LOW);
  delay(1000 / 64); // wait for a second
  for (char c = 'a'; c <= 'n'; ++c)
  {
    uart_write(c);
  }
}
