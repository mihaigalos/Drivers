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


# License
This software is freely available under the GNU GPL v3.0 aegis, please consult the LICENSE file for further information.