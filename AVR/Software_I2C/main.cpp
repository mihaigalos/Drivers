/*
 * SoftI2Clib_for_AVR.cpp
 *
 * Created: 2/21/2017 9:34:37 PM
 * Author : Mihai
 */ 

#include <avr/io.h>
#include "i2csoft.h"

int main(void)
{
	I2C i2c;
	i2c.Begin();
    /* Replace with your application code */
	
	/*DDRB = 0x01;
	while(1){
		PORTB = 0x01;
		PORTB = 0x00;	
	}*/
		
    while (1) {
		/*i2c.Start();
		i2c.WriteByte(0x42);
		i2c.WriteByte(0x43);
		i2c.Stop();*/
		
		i2c.StartTransmission(0x42);
		i2c.Write(0x45);
		_delay_ms(10);
    }
}

