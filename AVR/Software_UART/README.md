# AVR Software UART [![license](https://img.shields.io/badge/license-GPLv3-brightgreen.svg)](LICENSE)
A custom-made lightweight Software UART Library for the AVR

# Keywords 
C, asm, software uart library

# The gist
Own C implementation (assembler-optimized) Software UART for the AVR.
Easily customizable through the include header. Just set the F_CPU, FIXED_BAUD_RATE and the PORT macros to 
desired values and the library should do the rest. 

Tested at 9600bps 8N1 LSB first @ 1, 2, 4 and 8Mhz on a AtTiny85. Screenshots for 2Mhz :: 9600/8N1 available
in screenshots/.

# Usage 

1. Set the required macros
..* F_CPU to the microcontroller frequency
..* FIXED_BAUD_RATE to the desired communication baud rate
..* RX_PIN and TX_PIN to the reception an transmission pins. The pins need to be on the same port.
..* UART_DDR to the data direction register on the AVR
..* UART_IN_PORT_MAPPING to the input port mapping on the AVR. The pin specified in RX_PIN will be used 
for reception.
..** UART_OUT_PORT_MAPPING to the output port mapping on the AVR. The pin specified in TX_PIN will be used 
for transmission. 
2. Call uart_init().
3. Call uart_write(char), where char is a character (i.e. : 'a').

# License
This software is freely available under the GNU GPL v3.0 aegis, please consult the LICENSE file for further information.