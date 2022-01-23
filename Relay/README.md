Source: http://www.icstation.com/icstation-micro-channel-relay-module-control-relay-module-icse012a-p-4012.html

```
Serial port using method

These modules can use the serial port to connect with other communication devices. These modules can receive the Single-byte instruction from the upper monitor.
Upper Monitor          0x50        0x51
ICSE012A          0xAB
ICSE013A          0xAD
ICSE014A          0xAC

Suggested by our customer:

Here is the way it works, take ICSE012A as an example:
·   After the ICSE012A has been connected to PC, if send 0x50 to ICSE012A, it will reply 0xAB.( Relay Board Manager: Tool--Configure)
·   Then if you send 0x51 to ICSE012A, it is ready to receive hex code.( Relay Board Manager: Open the Sele)
Actually you can use a software “Bus Hound” to see what the transmission hex codes are and use Python PySerial library to customize the relay actions as well as “Relay Board Manager”.

```

### Python sample
```python
import serial
import time

fd=serial.Serial("COM3",9600)
time.sleep(1)
fd.write('x50')
time.sleep(0.5)
fd.write('x51')

def relay_1():
    fd.write('x00')
    time.sleep(1)
fd.write('x01')

if __name__ == "__main__":
relay_1()
```
