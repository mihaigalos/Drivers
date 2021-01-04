#! /bin/bash


# for building and testing on PC use:
# bazel build //...
# bazel test //...


# cross-compiling for AVR:

MCU=atmega328p

avr-gcc -D __AVR__  -fstack-usage -std=c++14 -mmcu="${MCU}" -Os main.cpp -o main.elf

avr-size --mcu="${MCU}" main.elf
echo ""
avr-size -C --mcu="${MCU}" main.elf

echo ""
avr-nm --size-sort -C -r --radix=d main.elf

echo ""
echo "Stack usage:"
cat main.su
