; SoftwareUart.S — cycle-accurate half-duplex software UART for AVR
; All timing derives from F_CPU and FIXED_BAUD_RATE at compile time.
;
; bitDelay(r18=K): burns exactly 3*K + 6 cycles inside function (rcall included).
; With the ldi K before the rcall: 3*K + 7 cycles total from ldi through ret.
;
; Per-bit timing budgets (both paths balanced):
;   TX: 3*K_TX + 14 = BIT_CYCLES  →  K_TX  = (BIT_CYCLES - 14) / 3
;   RX: 3*K_RX + 13 = BIT_CYCLES  →  K_RX  = (BIT_CYCLES - 13) / 3
;
; RX startup: sample center of bit-0 at 1.5 * BIT_CYCLES from falling edge.
;   Fixed overhead in startup path = 20 cycles.
;   K_HALF + K_START = (3*BIT_CYCLES/2 - 20) / 3
;   K_HALF ≈ (BIT_CYCLES/2 - 11) / 3  (samples mid-start-bit for glitch check)

#include <avr/io.h>
#include "SoftwareUart.h"

; ── Derived timing constants ──────────────────────────────────────────────────
#define BIT_CYCLES      (F_CPU / FIXED_BAUD_RATE)
#define K_TX            ((BIT_CYCLES - 14) / 3)
#define K_RX            ((BIT_CYCLES - 13) / 3)
#define K_HALF          ((BIT_CYCLES / 2 - 11) / 3)
#define K_START         ((BIT_CYCLES * 3 / 2 - 20) / 3 - K_HALF)

; Sanity checks
#if K_TX <= 0
#error K_TX is zero or negative. Decrease baud rate or increase F_CPU.
#endif
#if K_RX <= 0
#error K_RX is zero or negative. Decrease baud rate or increase F_CPU.
#endif

; ── Shared delay subroutine ───────────────────────────────────────────────────
; void bitDelay(void)  — count lives in r18, loaded by caller before rcall.
; Timing: rcall(3) + (K-1)*3 + 2 (last brne) + ret(4) = 3K + 6 inside func.
; Total from call site ldi through ret return: 3K + 7 cycles.
; Clobbers: r18 (call-clobbered per ABI — no push/pop needed).
; Does NOT touch r24/r25 or any call-saved register.
; Does NOT affect Z or C flags that callers depend on.

#if defined(RX_PIN) || defined(TX_PIN)
.global bitDelay
bitDelay:
    dec  r18
    brne bitDelay
    ret
#endif

; ── uart_write ────────────────────────────────────────────────────────────────
; void uart_write(uint8_t value)  — value arrives in r24 per AVR ABI.
; Frame: r25:r24 = 0x01:value  (16-bit sentinel, hi8=stop-bit marker).
; Each iteration: lsr r25 / ror r24 shifts frame right.
;   carry = transmitted bit (old r24.0)
;   Z flag = (r24 == 0 after ror) — used for loop exit via brne.
; Loop exits after 9 iterations: 8 data bits + 1 stop bit (hi8=0x01 → carry).
; Both pin-set paths cost equal cycles:
;   bit=1: sbi(1) + brcs-taken(2) + brne(2) = 5cc after delay
;   bit=0: sbi(1) + brcs-not-taken(1) + cbi(1) + brne(2) = 5cc after delay
; Total per bit: ldi(1) + bitDelay(3*K_TX+6) + lsr(1)+ror(1)+sbi(1)+brcs/cbi(2)+brne(2) = 3*K_TX+14

#ifdef TX_PIN
.global uart_write
uart_write:
    ldi  r25, 0x01                              ; hi8 sentinel (stop bit)
    cbi  _SFR_IO_ADDR(UART_OUT_PORT_MAPPING), TX_PIN  ; start bit: drive LOW

_txbit:
    ldi  r18, K_TX
    rcall bitDelay

    lsr  r25                                    ; shift hi8; old hi8.0 → carry
    ror  r24                                    ; rotate lo8; carry = data bit; Z = (r24==0)

    sbi  _SFR_IO_ADDR(UART_OUT_PORT_MAPPING), TX_PIN  ; set HIGH (unconditional)
    brcs _txdone                                ; carry set → bit=1: keep HIGH
    cbi  _SFR_IO_ADDR(UART_OUT_PORT_MAPPING), TX_PIN  ; carry clear → bit=0: pull LOW
_txdone:
    brne _txbit                                 ; Z from ror r24; loop until frame exhausted.
                                                ; line idles HIGH (stop bit = 1 was last sent)
    ret
#endif ; TX_PIN

; ── uart_read ─────────────────────────────────────────────────────────────────
; uint8_t uart_read(void)  — returns received byte in r24 per AVR ABI.
; Sentinel: r24 = 0x80. Each iteration: lsr r24 shifts sentinel toward carry.
;   When sentinel bit falls into carry (after 8 iterations), brcc exits loop.
;   Received bits are ORed into bit7 as the sentinel shifts down.
; Both pin-sample paths cost equal cycles:
;   pin=HIGH: sbis-skip(2) + ori(1) + ldi(1)+rcall bitDelay(3*K_RX+6) + brcc(2) = 3*K_RX+13
;   pin=LOW:  sbis-no-skip(1) + rjmp(2) + ldi(1)+rcall bitDelay(3*K_RX+6) + brcc(2) = 3*K_RX+13
; Startup aligns first sample to center of bit 0:
;   T_sample = 3*(K_HALF + K_START) + 20 ≈ 1.5 * BIT_CYCLES  (within 1 cycle)

#ifdef RX_PIN
.global uart_read
uart_read:
    ldi  r24, 0x80                              ; sentinel + accumulator

_detect:
    sbic _SFR_IO_ADDR(UART_IN_PORT_MAPPING), RX_PIN   ; spin while HIGH (idle)
    rjmp _detect

    ldi  r18, K_HALF                            ; delay to mid-start-bit
    rcall bitDelay

    sbic _SFR_IO_ADDR(UART_IN_PORT_MAPPING), RX_PIN   ; glitch check: skip if still LOW
    rjmp _detect                                ; was HIGH = glitch; restart

    ldi  r18, K_START                           ; delay from mid-start-bit to center of bit 0
    rcall bitDelay

_rxbit:
    lsr  r24                                    ; sentinel → carry after 8 iters; accumulator shifts
    sbis _SFR_IO_ADDR(UART_IN_PORT_MAPPING), RX_PIN   ; skip next if pin HIGH (bit = 1)
    rjmp _rxskip
    ori  r24, 0x80                              ; bit = 1: set MSB
_rxskip:
    ldi  r18, K_RX
    rcall bitDelay
    brcc _rxbit                                 ; carry clear = sentinel not yet at bit0; loop

    ret                                         ; r24 = received byte
#endif ; RX_PIN
