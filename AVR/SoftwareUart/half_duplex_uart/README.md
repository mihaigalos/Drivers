([Original article](http://nerdralph.blogspot.com/2014/01/avr-half-duplex-software-uart.html))

![alt text](half_duplex_uart.png)

When the TTL serial adapter is not transmitting, the voltage from the Tx pin keeps Q1 turned on, and the AVR pin (Tx/Rx) will sense a high voltage, indicating idle state.  When the AVR transmits a 0, with Q1 on, Rx will get pulled low indicating a 0.  R1 ensures current flow through the base of Q1 is kept below 1mA.  When the AVR transmits a 1, Rx will no longer be pulled low, and Rx will return to high state.  When the serial adapter is transmitting a 0, D1 will allow it to pull the AVR pin low.  With no base current, Q1 will be turned off, and the Rx line on the serial adapter will be disconnected from the transmission.
