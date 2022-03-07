([Original article](http://nerdralph.blogspot.com/2014/01/avr-half-duplex-software-uart.html))

```
When using this half-duplex method for software UART _between two microcontrollers_, a pullup is necessary on the Tx/Rx line.
The pullup ensures the line is High when the right Tx is Low (transistor blocked).
This pullup is already present in harware implementation of serial communication devices, in that case no pullup is needed.
```

![alt text](half_duplex_uart.png)

##### TTL Serial

Tx |State 
---|--------
 0 | T blocked, AVR pin 0 (because of D)
 1 | T on, AVR pin 1 (because of internal pullup)


##### AVR
Tx | State
---| -------
 0 | T on, Rx 0
 1 | T on(because of D), Rx 1

When the TTL serial adapter is not transmitting, the voltage from the Tx pin keeps Q1 turned on, and the AVR pin (Tx/Rx) will sense a high voltage, indicating idle state.  When the AVR transmits a 0, with Q1 on, Rx will get pulled low indicating a 0.  R1 ensures current flow through the base of Q1 is kept below 1mA.  When the AVR transmits a 1, Rx will no longer be pulled low, and Rx will return to high state.  When the serial adapter is transmitting a 0, D1 will allow it to pull the AVR pin low.  With no base current, Q1 will be turned off, and the Rx line on the serial adapter will be disconnected from the transmission.

##### Simplified 1-Diode

This schematic can be used when interfacing different voltage levels (i.e.: 5V TTL with 3.3V CMOS).

![alt text](one_wire.png)


##### Simplified Single Resistor

Use this when `A` and `B` are connected to same VCC and same GND.
If `B` has an internal pull-up resistor Rpu = 10kΩ, and assuming maximum input Voltage for a logic '0' Vil_max = 0.8V (just to be sure), we need the following resistor value R to reliably drive a '0': (voltage divider)

```
   5V * R / (R + 10kΩ) <= 0.8V
                     R <= 1.9kΩ
```

If `B` does not have an internal pull-up, use a value for the resistor = 10kΩ..100kΩ. Your `A` Tx will drive a '1' on bus idle, which will conveniently pull the bus up to VCC when your state machine logic expects B to answer.

In any case, the resistor will prevent shorts in case the `B` and your controller drive the bus at the same time. It will also act as a pull-up for your own Rx line, so you won't get spurious data input when `B` isn't connected.

One can use a multimeter to determine which of the two cases your application falls in. When in doubt, start with 1.8kΩ, see that your code works. Then move on to 100kΩ. If your code fails, you can be sure they use an internal Rpu, so stay 1.8kΩ.

![alt text](single_resistor.png)
