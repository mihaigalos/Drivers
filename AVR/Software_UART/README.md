# AVR Software UART [![license](https://img.shields.io/badge/license-GPLv3-brightgreen.svg)](LICENSE)
A custom-made lightweight Software UART Library for the AVR

# Keywords
C, asm, software uart library

# The gist
Own C implementation (assembler-optimized) Software UART for the AVR.
Easily customizable through the include header.

Receive not yet ready. Please do not use uart_read().

Send tested successfully on a AtTiny85 at:
  * 9600bps   8N1 big endian @ 1, 2, 4 and 8Mhz.
  * 115200 bps  8N1 big endian @ 8Mhz.
  * 230400 bps  8N1 big endian @ 16Mhz.

Screenshots for 2Mhz :: 9600/8N1 available
in screenshots/.

# Usage

1. Set the required macros
  * F_CPU to the microcontroller frequency.
  * FIXED_BAUD_RATE to the desired communication baud rate.
  * RX_PIN and TX_PIN to the reception an transmission pins. The pins need to be on the same port.
  * UART_DDR to the data direction register on the AVR.
  * UART_IN_PORT_MAPPING to the input port mapping on the AVR. The pin specified in RX_PIN will be used
for reception.
  * UART_OUT_PORT_MAPPING to the output port mapping on the AVR. The pin specified in TX_PIN will be used
for transmission.
2. Call uart_init().
3. Call uart_write(char), where char is a character (i.e. : 'a').

# Size Requirements

**Flash ROM**

Rom usage has been computed using the following command:

```avr-nm --size-sort -C -r --radix=d  attiny10-software-uart-master.elf ```    



```
00000056 T uart_write(unsigned char)
00000008 T uart_init()

64 bytes Total.
```

RAM
```
4 bytes required for stack pushes of PC during calls.
```

# License
This software is freely available under the GNU GPL v3.0 aegis, please consult the LICENSE file for further information.
