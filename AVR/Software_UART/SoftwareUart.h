/*
 * SoftwareUart.h
 *
 * Created: 4/20/2017 11:40:05 PM
 *  Author: Mihai Galos
 */ 


#ifndef UART_READ_H_
#define UART_READ_H_

#include <avr/io.h>

#define F_CPU 1000000UL             // Frequency of the CPU of the microcontroller
#define FIXED_BAUD_RATE (9600UL)    // Desired Baudrate

#define RX_PIN 0
#define TX_PIN 1

#define UART_DDR DDRB                   // Data direction for port
#define UART_IN_PORT_MAPPING PINB       // Mapping of UART to physical input port
#define UART_OUT_PORT_MAPPING PORTB



void uart_init();
uint8_t uart_read();
void uart_write(uint8_t value);


#endif /* UART_READ_H_ */