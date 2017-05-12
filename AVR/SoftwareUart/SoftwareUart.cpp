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
 * SoftwareUart.cpp
 *
 * Created: 4/20/2017 9:29:03 PM
 *  Author: Mihai Galos
 */

#include "SoftwareUart.h"

#ifndef F_CPU
#error Please #define F_CPU
#endif

// Internal computing macros
#define BIT_TIME_NANOSEC 1000000000UL / FIXED_BAUD_RATE
#define ONE_CLOCK_CYCLE_NANOSEC (1000000000UL / F_CPU)
#define CLOCK_CYCLES_PER_FULL_WAIT_LOOP 3 // using 3cc for each iteration
#define PRESCALE_WAIT_ONE_BIT                                                  \
  ((BIT_TIME_NANOSEC / ONE_CLOCK_CYCLE_NANOSEC) /                              \
   (CLOCK_CYCLES_PER_FULL_WAIT_LOOP))
#define PRESCALE_WAIT_HALF_BIT

#define PRESCALE_WAIT_ONE_BIT_RX_NO_OFFSET PRESCALE_WAIT_ONE_BIT
#define INSTRUCTION_OFFSET_RX                                                  \
  8 // clock cycles needed after reading a pin hi/lo before starting
    // bitDelaySend

#define PRESCALE_WAIT_ONE_BIT_RX                                               \
  (PRESCALE_WAIT_ONE_BIT_RX_NO_OFFSET - INSTRUCTION_OFFSET_RX)
#define PRESCALE_WAIT_HALF_BIT_RX                                              \
  (static_cast<uint8_t>(PRESCALE_WAIT_ONE_BIT_RX)) / 2

#define INSTRUCTION_OFFSET_TX                                                  \
  7 // clock cycles needed after setting a pin hi/lo before starting
    // bitDelaySend
#define PRESCALE_WAIT_ONE_BIT_TX (PRESCALE_WAIT_ONE_BIT - INSTRUCTION_OFFSET_TX)

#if PRESCALE_WAIT_ONE_BIT_RX == 0
#ifdef F_CPU
#error PRESCALE_WAIT_ONE_BIT_RX is 0. Try decreasing the Baudrate.
#endif
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#pragma message "assuming clock: " STR(F_CPU) " Mhz."

#if defined(RX_PIN) || defined(TX_PIN)
void uart_init() {
#ifdef TX_PIN
  UART_DDR |= (1 << TX_PIN);
  UART_OUT_PORT_MAPPING |= (1 << TX_PIN); // Tx line high when idle
#endif

#ifdef RX_PIN
  UART_DDR &= ~(1 << RX_PIN);
#endif
}
#endif // #if defined (RX_PIN) || defined (TX_PIN)

#ifdef RX_PIN

uint8_t uart_read() {
  uint8_t temporary = 0;
  uint8_t readValue = 0;
  uint8_t bitPosition = 0x40;

  __asm__ volatile(
            
        "wait: \n\t"
            "sbic %6, %1\n\t"
                "rjmp wait \n\t"
        "rcall halfDelay \n\t"  
                        
        "sbic %6, %1\n\t"
            "rjmp wait \n\t" 
        "rcall bitDelayReceive \n\t"

        "read8bits: \n\t"   
                        
        "sbic %6, %1\n\t"       // 2cc true, 1cc false
            "rjmp setBit\n\t"       
        "rjmp skipBitSet\n\t"   
                        
        "setBit:   \n\t"
            "ori %0, 0x80\n\t" 

        "skipBitSet: \n\t"
            "rcall bitDelayReceive \n\t"
                       
        "lsr %0 \n\t"
        "lsr %3 \n\t"
            
        "breq eof_read8bits \n\t"
            "rjmp read8bits \n\t"
			        
        // delay routines
			       
        "halfDelay: \n\t"
            "ldi %2, %4 \n\t"
            "rjmp loop_3cc \n\t"
        "bitDelayReceive: \n\t"
            "mov %2, %5 \n\t"
        "loop_3cc: \n\t"
            "dec %2 \n\t"            // 1cc
            "brne loop_3cc \n\t"      // 2cc (true), 1cc (false)
            "ret \n\t"
			       
        // done
        "eof_read8bits: \n\t" 
                                     
		: "=&r"(readValue)
        : "M"(RX_PIN), "r"(temporary), "r"(bitPosition),
        "M"(PRESCALE_WAIT_HALF_BIT_RX),
        "r"(PRESCALE_WAIT_ONE_BIT_RX),
        "M"(_SFR_IO_ADDR(UART_IN_PORT_MAPPING))

            );
  return readValue;
}
#endif // #ifdef RX_PIN

#ifdef TX_PIN
void uart_write(uint8_t value) {
  uint8_t temporary = 0, bitsRemaining = 8; // 8 bits
  __asm__ volatile(

      "cbi %4, %0 \n\t" // falling edge : start condition
      "rcall bitDelaySend \n\t"
     
      "write8bits: \n\t"
          "mov %5, %1 \n\t"
          "andi %1, %2 \n\t"
      "breq setPinLow \n\t"      // and == 0, meaning LSB was 0, set bit low
      "sbi %4, %0 \n\t"          // and == 1, meaning LSB was 1, set bit high
      "rjmp pinSetFinished \n\t" // .. and jump to pinSetFinished
      
      "setPinLow: \n\t"
          "cbi %4, %0 \n\t"
          "nop \n\t" // additional no operation to balance the clock cycle count
                     // when taking the previous branches
      "pinSetFinished:"
          "mov %1, %5 \n\t"
          "lsr %1 \n\t"
          "rcall bitDelaySend \n\t"
          "dec %3 \n\t"
          "brne write8bits \n\t"
          "rjmp eof_write8bits \n\t"

      "bitDelaySend: \n\t"
        "mov %5, %6 \n\t"
      "repeat_3cc: \n\t"
          "dec %5 \n\t"          // 1cc
          "brne repeat_3cc \n\t" // 2cc (true), 1cc (false)
          "ret \n\t"
      
      // done
      "eof_write8bits: \n\t"
          "sbi %4, %0 \n\t" // stop bit
          "rcall bitDelaySend \n\t"
      :
      : "M"(TX_PIN), "r"(value), "M"(0x01), "r"(bitsRemaining),
        "M"(_SFR_IO_ADDR(UART_OUT_PORT_MAPPING)), "r"(temporary),
        "r"(PRESCALE_WAIT_ONE_BIT_TX)

          );
}
#endif // #ifdef TX_PIN