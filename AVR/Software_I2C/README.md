These days, most people use Arduino for their HW projects. Fair enough, me too.

Some microcontrollers are not supported by it, however. Some others have no hardware support for I2C altogether.
So.. here's a (generic) implementation of a software I2C
on any two pins of an AVR, manually calibrated with an oscilloscope for various frequencies.

Similar to the Wire library, this library is still in its infancy and will be (hopefully) developed further.

Original (base) implementation can be found at http://extremeelectronics.co.in/avr-tutorials/software-i2c-library-for-avr-mcus/


