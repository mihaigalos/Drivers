# AVR Software UART [![license](https://img.shields.io/badge/license-GPLv3-brightgreen.svg)](LICENSE)
A custom-made lightweight Software UART Library for the AVR.

# Keywords
C, asm, avr assembly, software uart library

# The gist
Cycle-accurate AVR software UART (send and receive) implemented in hand-written AVR assembly
(`SoftwareUart.S`). Easily customizable through the include header. Selective compilation: comment
out `RX_PIN` or `TX_PIN` to strip the unused direction entirely.

# Screenshot
Transmit, receive and retransmit demo screenshot on oscilloscope.
![alt text](screenshots/self_test_sw_uart_receive_send.png "Demo Transmit and Receive")

# Tests
Send tested successfully on an AtTiny85:
  * 9600bps     8N1 lsb-first @ 1, 2, 4 and 8Mhz.
  * 115200 bps  8N1 @ 8Mhz.
  * 230400 bps  8N1 @ 16Mhz.

Receive and send tested successfully on an AtTiny10:
  * 38400bps    8N1 lsb-first @ 8Mhz (requires setting CCP = 0xD8 and CLKPSR=0).

Screenshots for Tiny85::2Mhz :: 9600/8N1 send available in screenshots/self_test_sw_uart_send.png

Screenshots for Tiny10::8Mhz :: 38400/8N1 s&r available in screenshots/self_test_sw_uart_receive_send.png

# Usage

1. Set the required macros in `SoftwareUart.h`:
  * `F_CPU` — microcontroller clock frequency in Hz.
  * `FIXED_BAUD_RATE` — desired baud rate.
  * `RX_PIN` / `TX_PIN` — pin numbers on the port. Comment out either to exclude that direction and save flash.
  * `UART_DDR` — data direction register (e.g. `DDRB`).
  * `UART_IN_PORT_MAPPING` — input pin register (e.g. `PINB`). RX and TX must be on the same port.
  * `UART_OUT_PORT_MAPPING` — output port register (e.g. `PORTB`).
2. Call `uart_init()` — zero flash cost, always inlined.
3. Send or receive:
  * `uart_write(uint8_t)` — transmit one byte.
  * `uart_read()` — block until one byte is received, return it.

# Implementation notes

All timing constants are derived at compile time from `F_CPU` and `FIXED_BAUD_RATE`. No runtime
calibration or timer hardware is used. The shared `bitDelay` subroutine (3 words) is called by
both TX and RX, eliminating duplicated delay loops.

**TX** uses a 16-bit sentinel frame (`hi8 = 0x01 | data`). Each iteration shifts the frame right;
the loop exits automatically when the frame is exhausted — stop bit is transmitted implicitly with
no extra code.

The sentinel frame is necessary because a plain 8-bit counter (`dec / brne`) would require a
separate register and two extra instructions per loop iteration, plus an out-of-loop stop-bit
section — costing ~10 extra bytes. The problem with a naked data byte is also that `0x00` would
exit the loop immediately on a `brne`-based test, corrupting transmission of null bytes. The
16-bit frame solves both problems in one: the data occupies `lo8` and the `0x01` sentinel in
`hi8` acts as a self-terminating marker. After 9 right-shifts (`lsr r25` / `ror r24`), `lo8`
reaches zero only once the sentinel has been shifted all the way through — guaranteeing exactly 8
data bits are sent regardless of data value, and transmitting the stop bit (the sentinel `1` bit)
as the final shift before the loop exits. No counter register, no stop-bit section, no null-byte
bug.

**RX** uses a single `0x80` sentinel in the accumulator. Right-shifting moves the sentinel toward
carry; when it falls out of bit 0 (after exactly 8 iterations) the carry flag exits the loop.
A half-bit glitch filter on the start bit rejects pulses shorter than half a bit period.

Timing accuracy: ±1 cycle per bit (0.24% at 38400/16MHz), well within UART margin.

# Size Requirements

**Flash ROM** (estimated, `-Os`, all functions enabled)

```
; Function          Words  Bytes
; uart_write          11    22    TX + shared delay
; uart_read           17    34    RX
; bitDelay             3     6    shared (counted once)
; uart_init            0     0    always_inline, no text symbol

~62 bytes Total TRX
~28 bytes TX only  (RX_PIN commented out)
~40 bytes RX only  (TX_PIN commented out)
```

Previous version (v1, inline ASM in .cpp): 120 bytes TRX.

**RAM**

2 bytes of stack for the `PC` push during `bitDelay` calls. No static variables.

# License
This software is freely available under the GNU GPL v3.0 aegis, please consult the LICENSE file for further information.
