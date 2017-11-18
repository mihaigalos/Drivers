Adafruit_mfGFX_Library
======================

Multifont version of Adafruit_GFX library with Adafruit ST7735 Display drivers and demo.

This library supports any compatible Adafruit_GFX display driver.  The free TheDotFactory Windows program is recommended for created data compatible with the Adafruit_mfGFX library.

Please see the README.pdf files for instructions on creating and adding new fonts.  The base library comes with 4 fonts and a "test" font so new candidate font data can easily be added and tested before being permanently added to the library.

BE AWARE that though font data does not take up Spark RAM, they do however take up flash space so having too many fonts may prove problematic.

** UPDATED with font compilation option, June 2014

This is a library for the Adafruit 1.8" SPI display.
This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
Check out the links above for our tutorials and wiring diagrams.
These displays use SPI to communicate, 4 or 5 pins are required
to interface (RST is optional).
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

!!! mfGFX library adapted from Adafruit GFX library by Paul Kourany, 2014 !!!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution

To download. click the DOWNLOADS button in the top right corner, rename the uncompressed folder Adafruit_ST7735. Check that the Adafruit_ST7735 folder contains Adafruit_ST7735.cpp and Adafruit_ST7735.

Place the Adafruit_ST7735 library folder your <arduinosketchfolder>/libraries/ folder. You may need to create the libraries subfolder if its your first library. Restart the IDE

