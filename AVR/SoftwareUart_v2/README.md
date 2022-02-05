### SoftwareUart_v2

Simplified, more portable bit-bang software uart, 64 bytes.
Uses clock scaling to achieve baudrate.

Reception (read) is currently only working for individual characters, as it is not fast enough for multi-character communication.
Should work fine, however, if one adds a small delay on the transmitter side after each byte sent.

**Warning: Cycle accuracy is only guaranteed and successfuly tested on AtTiny85. Actual observed bitrate for requested 38400bps is 25200bps on AtMega328p.**

**TODO: make uart_read() an Iterrupt Service Routine and write to a global buffer if count < capacity. The _if_ should substitute the clock cycles needed for poll-waiting for the line to go low. The actual writing to the buffer can take place during the STOP bit.**

```
$ avr-nm --size-sort -C -r --radix=d *.elf

00000028 T uart_write(unsigned char)
00000020 T uart_read()
00000016 T uart_init()
```

By using a clock division factor to slow the clock cycles down, we don't need a delay when bit-banging through software uart.

If we use the default clock of 16Mhz, and divide it by a division factor of 64 (CLKPS register on AtTiny10), we get a clock of 250Khz.

```
1/250000 = 4us/cc
4us/cc * 7cc/bit (computed for Tx by the generated assembly)=28us/bit
28us/bit * 8 bit/byte = 224 us/byte
```
Also, at 28us/bit, this would yield a bitrate of 1/0.000028 = 35714 bits/s.
This bitrate is close enough to 38400 for most practical purposes.

### Results

Successful transmission (write) on AtTiny85 at 38400@8N1. Clock reduced using div64.

![alt text](screenshots/38400_tx_high_accuracy_nops_enabled.png "Demo Transmit and Receive")

