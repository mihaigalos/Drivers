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

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#pragma message "assuming clock: " STR(F_CPU) " Mhz."

// Raw cycles per bit
#define BIT_CYCLES  (F_CPU / FIXED_BAUD_RATE)

// TX: delay-first loop structure.
// Per-bit overhead: lsr(1)+ror(1)+brcc+sbi/cbi+rjmp/nop(4)+brne(2) = 8cc (both paths balanced)
#define TX_BIT_OVERHEAD  8
#define TX_BIT_WAIT      (BIT_CYCLES - TX_BIT_OVERHEAD)

// RX: sample-first loop structure.
// Per-bit overhead: lsr(1)+sbis+rjmp/ori(3)+brcc(2) = 6cc (both paths balanced)
// carry flag is NOT clobbered by dec/brne inside __builtin_avr_delay_cycles
#define RX_BIT_OVERHEAD  6
#define RX_BIT_WAIT      (BIT_CYCLES - RX_BIT_OVERHEAD)

// RX first-bit alignment: after glitch-check (half-bit + ~4cc overhead consumed),
// wait remaining distance to center of bit 0 = BIT_CYCLES - 4
#define RX_START_WAIT    (BIT_CYCLES - 4)

#if TX_BIT_WAIT <= 0
#error TX_BIT_WAIT is zero or negative. Decrease baud rate or increase F_CPU.
#endif
#if RX_BIT_WAIT <= 0
#error RX_BIT_WAIT is zero or negative. Decrease baud rate or increase F_CPU.
#endif

#ifdef RX_PIN

uint8_t uart_read()
{
  // r24 = ABI return register; c serves as both sentinel counter and accumulator.
  // Sentinel 0x80 shifts right each iteration; carry sets after 8 shifts → exit.
  register uint8_t c asm("r24") = 0x80;

detect:
  // Wait for start bit falling edge (sbic/rjmp = 2cc loop)
  __asm__ volatile(
      "0: sbic %[pin_reg], %[rx_pin] \n\t"
      "   rjmp 0b \n\t"
      :: [pin_reg] "M"(_SFR_IO_ADDR(UART_IN_PORT_MAPPING)), [rx_pin] "M"(RX_PIN)
  );

  // Half-bit glitch check: sample at mid-start-bit; if HIGH it was a glitch
  __builtin_avr_delay_cycles(BIT_CYCLES / 2);
  if (UART_IN_PORT_MAPPING & (1 << RX_PIN)) goto detect;

  // Align to center of first data bit
  __builtin_avr_delay_cycles(RX_START_WAIT);

rxbit:
  // Sample pin, accumulate bit into c, shift sentinel toward carry
  __asm__ volatile(
      "lsr %[c] \n\t"                          // c >>= 1; carry = old bit0 (sentinel)
      "sbis %[pin_reg], %[rx_pin] \n\t"        // skip if pin HIGH (bit=1)
      "rjmp 1f \n\t"                           // pin LOW: skip ori
      "ori %[c], 0x80 \n\t"                    // pin HIGH: set MSB
      "1: \n\t"
      : [c] "+r"(c)
      : [pin_reg] "M"(_SFR_IO_ADDR(UART_IN_PORT_MAPPING)), [rx_pin] "M"(RX_PIN)
  );
  // delay AFTER sample; dec inside __builtin doesn't touch carry flag
  __builtin_avr_delay_cycles(RX_BIT_WAIT);
  // brcc: loop while carry clear (sentinel not yet shifted out = fewer than 8 bits read)
  asm goto("brcc %l[rxbit]" :::: rxbit);

  return c;
}

#endif // #ifdef RX_PIN

#ifdef TX_PIN

void uart_write(uint8_t value)
{
  // 16-bit sentinel frame: lo8 = data, hi8 = 0x01 (stop bit).
  // Each iteration: lsr r25 / ror r24 shifts frame right; carry = transmitted bit.
  // Loop exits via brne when lo8 (r24) reaches 0 after 9 shifts (8 data + 1 stop).
  // value arrives in r24 per ABI; compiler emits ldi r25, 0x01 for hi8.
  register uint16_t frame asm("r24") = ((uint16_t)0x01 << 8) | value;

  // Start bit: drive TX LOW atomically (1cc)
  asm volatile(
      "cbi %[port], %[pin] \n\t"
      :: [port] "M"(_SFR_IO_ADDR(UART_OUT_PORT_MAPPING)), [pin] "M"(TX_PIN)
  );

txbit:
  // Delay first: Z flag from ror below will be fresh for brne
  __builtin_avr_delay_cycles(TX_BIT_WAIT);

  __asm__ volatile(
      "lsr r25 \n\t"                           // shift hi8; carry = hi8[0]
      "ror r24 \n\t"                           // rotate lo8; carry = old lo8[0] = bit to send
                                               // Z flag = (new r24 == 0) — used by brne below
      "brcc 1f \n\t"                           // carry clear → bit=0
      "sbi %[port], %[pin] \n\t"               // bit=1: drive HIGH
      "rjmp 2f \n\t"
      "1: \n\t"
      "cbi %[port], %[pin] \n\t"               // bit=0: drive LOW
      "nop \n\t"                               // balance: brcc(2)+cbi(1)+nop(1) = brcc(1)+sbi(1)+rjmp(2) = 4cc
      "2: \n\t"
      : "+r"(frame)
      : [port] "M"(_SFR_IO_ADDR(UART_OUT_PORT_MAPPING)), [pin] "M"(TX_PIN)
  );
  // brne reads Z set by ror r24; sbi/cbi/rjmp/nop do not modify Z
  asm goto("brne %l[txbit]" :::: txbit);
  // line remains HIGH (last transmitted bit was stop bit = 1) = UART idle state
}

#endif // #ifdef TX_PIN