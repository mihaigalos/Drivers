### SoftwareUart_v2 (Work in Progress)

Simplified, portable bit-bang software uart.

By using a clock division facktor to slow the clock cycles down, we don't need a delay when bit-banging through software uart.

For AVR, sending requires :
* 1 cc for loading the value 8 to know how many bits had been sent.
* 8 cc / loop of sending 1 bit.
* 1 extra cc when the branch is false.
* 2 cc for setting the output to default high.
* 4 cc for returning.

```asm
.file	"SoftwareUart.cpp"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__CCP__ = 0x3c
__tmp_reg__ = 16
__zero_reg__ = 17
	.text
.global	_Z10uart_writeh
	.type	_Z10uart_writeh, @function
_Z10uart_writeh:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */



.L__stack_usage = 0
	ldi r20,lo8(8)  //1 cc
.L2:
	mov r21,r24     //1
	andi r21,lo8(1) //1
	lsl r21         //1
	lsl r21         //1
	out 0x2,r21     //1
	lsr r24         //1
	subi r20,lo8(-(-1)) //1
	brne .L2        //1 or 2
	sbi 0x2,2       //2
	ret             //4
	.size	_Z10uart_writeh, .-_Z10uart_writeh
	.ident	"GCC: (GNU) 5.4.0"
```

If we use the default clock of 8Mhz, and divide it by a division factor of 64 (CLKPS register on AtTiny10), we get a clock of 125Khz.

1/125000 = 8us/cc
8us/cc * 8cc/bit (computed by the generated assembly)=64us/bit
64us/bit * 8 bit/byte = 512 us/byte

Also, at 64us/bit, this would yield a bitrate of 1/0.000064 = 15625 bits/s.