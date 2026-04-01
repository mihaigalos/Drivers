/*****************************************************************************
*
* This file is part of SoftwareUart.
*
* SoftwareUart is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* SoftwareUart is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SoftwareUart.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************************/

/*
 * SoftwareUart.h
 *
 * Created: 4/20/2017 11:40:05 PM
 *  Author: Mihai Galos
 */

#ifndef UART_READ_H_
#define UART_READ_H_

#include <avr/io.h>

#define F_CPU 16000000UL          // Frequency of the CPU of the microcontroller
#define FIXED_BAUD_RATE (38400UL) // Desired Baudrate

// Pins used in writing and reading to/from the UART. Comment out if not needed
// to save up Flash space.
#define RX_PIN 1
#define TX_PIN 0

#define UART_DDR DDRB             // Data direction for port
#define UART_IN_PORT_MAPPING PINB // Mapping of UART to physical input port
#define UART_OUT_PORT_MAPPING PORTB

#if defined(RX_PIN) || defined(TX_PIN)
static inline __attribute__((always_inline)) void uart_init()
{
#ifdef TX_PIN
  UART_DDR |= (1 << TX_PIN);
  UART_OUT_PORT_MAPPING |= (1 << TX_PIN); // Tx line high when idle
#endif
#ifdef RX_PIN
  UART_DDR &= ~(1 << RX_PIN);
#endif
}
#endif

uint8_t uart_read();
void uart_write(uint8_t value);

#endif /* UART_READ_H_ */
