RFM69_softSPI Library
=====================

Original work of Grzegorz Miasko, Felix Rusu.

This library allows you to use RFM69 radio module with Arduino over software SPI while using another SPI peripheral.
It is a modified version of RFM69 library by Felix Rusu (LowPowerLab), but it uses software SPI addressable RFM69 instance like in RF24_softSPI (sorry, I don't know it's author).
The SoftSPI library (part of DigitalIO library) is by William Greiman.

To use this library: download zip file and unpack it. Change the name of created folder to RFM69. Copy this folder to your Arduino additional Libraries folder. If you already have DigitalIO library you need to copy RFM69*.* and keywords.txt files only (and example folder if you wish). Then select 'Import Library' in Arduino IDE like you would for any other library.

The default settings are:
- MISO	- pin 6,
- MOSI	- pin 5,
- SCK		- pin 4,
- NSS		- pin 7,

but you can use another pins - change this by going into your RFM69.h file.

Note: Don't delete the original RFM69 library folder if you had it - just move or rename it. If you want to use hardware SPI again you have to return to the regular version of RFM69 library.

Additionaly at example folder you find the Fritzing images show the connections of RFM69 module to Arduino (Mega, Pro Mini).
