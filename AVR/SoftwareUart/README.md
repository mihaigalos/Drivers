# AVR Software UART [![license](https://img.shields.io/badge/license-GPLv3-brightgreen.svg)](LICENSE)
A custom-made lightweight Software UART Library for the AVR.

# Keywords
C, asm, software uart library

# The gist
Own C implementation (assembler-optimized) Software UART (send and receive) for the AVR.
Easily customizable through the include header.

Send tested successfully on an AtTiny85:
  * 9600bps     8N1 big endian (lsb-first) @ 1, 2, 4 and 8Mhz.
  * 115200 bps  8N1 @ 8Mhz.
  * 230400 bps  8N1 @ 16Mhz.

Receive and send tested successfully on an AtTiny10:
  * 38400bps    8N1 big endian (lsb-first) @ 8Mhz (requires setting CCP = 0xD8 and CLKPSR=0).
  * higher bps currently not working (need to calibrate the delay offset INSTRUCTION_OFFSET_RX).
    
Screenshots for Tiny85::2Mhz :: 9600/8N1 send available in screenshots/self_test_sw_uart_send.png 

Screenshots for Tiny10::8Mhz :: 38400/8N1 s&r available in screenshots/self_test_sw_uart_receive_send.png 

# Usage

1. Set the required macros.
  * F_CPU to the microcontroller frequency.
  * FIXED_BAUD_RATE to the desired communication baud rate.
  * RX_PIN and TX_PIN to the reception an transmission pins. The pins need to be on the same port.
  * UART_DDR to the data direction register on the AVR.
  * UART_IN_PORT_MAPPING to the input port mapping on the AVR. The pin specified in RX_PIN will be used
for reception.
  * UART_OUT_PORT_MAPPING to the output port mapping on the AVR. The pin specified in TX_PIN will be used
for transmission.
2. Call uart_init().
3. Send or receive
  * Call uart_write(char), where char is a character (i.e. : 'a').
  * Call uart_read() to get a character from the UART interface.

# Size Requirements

**Flash ROM**

Rom usage has been computed using the following command:

```avr-nm --size-sort -C -r --radix=d  attiny10-software-uart-master.elf ```    



```
00000056 T uart_read()
00000056 T uart_write(unsigned char)
00000008 T uart_init()

120 bytes Total.
```

**RAM**

Up to 5 bytes required for stack pushes of PC during calls and local variables. If only reading from UART is required,
RAM usage is 4 bytes.


# License
This software is freely available under the GNU GPL v3.0 aegis, please consult the LICENSE file for further information.
