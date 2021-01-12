#! /bin/sh

avr-gcc -std=c++14 -fstack-usage -save-temps -I. -mmcu=attiny85 -Os *.cpp *.cxx -o  software_uart.elf
avr-objcopy -O ihex -R .eeprom *.elf out.hex

