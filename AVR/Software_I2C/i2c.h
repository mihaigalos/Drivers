/**********************************************************
Software I2C Library for AVR.

Manually calibrated for common frequencies.
Copyright Mihai Galos 2017
**********************************************************/

#ifndef _I2CSOFT_H
#define _I2CSOFT_H

#define F_CPU 16000000L
#include <util/delay.h>

#define SPEED_KHZ 300 // kHz
#if SPEED_KHZ == 200
#define MANUAL_CLK_OFFSET_USEC -0.28
#elif SPEED_KHZ == 300
#define MANUAL_CLK_OFFSET_USEC -0.3 // manual fine-tuning, can be 0. This is to compensate for the bit shifts inside the sending and receiving methods
#elif SPEED_KHZ == 400
#define MANUAL_CLK_OFFSET_USEC -0.33
#endif

#define SCLPORT	PORTB	
#define SCLDDR	DDRB	

#define SDAPORT	PORTB	
#define SDADDR	DDRB	

#define SDAPIN	PINB	
#define SCLPIN	PINB	

#define SCL	0		
#define SDA	1		

#define SDA_LOW		SDAPORT&=(~(1<<SDA))
#define SDA_HIGH	SDAPORT|=((1<<SDA))

#define SCL_LOW		SCLPORT&=(~(1<<SCL))
#define SCL_HIGH	SCLPORT|=((1<<SCL))

#define MAX_BUFFER_SIZE 32

typedef enum {
	direction_read=0,
	direction_write
}EI2C_ReadWrite;

class I2C{
	uint8_t buffer_[MAX_BUFFER_SIZE];
	uint8_t buffer_pos_;
	bool start_acknowledged_;
public:

	//	Setup low-level I/O to a kown state
	void Begin();	

	// Generates a START Condition.
	void Start();

	// Generates a STOP Condition.
	void Stop();

	// Sends a Byte to the slave. Returns a true if slave acknowledged, false otherwise.
	bool WriteByte(uint8_t data);

	// Reads a byte from slave. 
	// Args:
	// 	1 if you want to acknowledge the receipt to slave.
	// 	0 if you don't want to acknowledge the receipt to slave.
	// Returns:
	// 	The 8 bit data read from the slave.

	uint8_t ReadByte(uint8_t ack);
	
	// Arduino-compatible methods
	
	void StartTransmission(uint8_t address, EI2C_ReadWrite rw=direction_write);
	uint8_t RequestFrom(uint8_t address, uint8_t count);
	void Write(uint8_t data);
	uint8_t EndTransmission();
	
};
#endif 