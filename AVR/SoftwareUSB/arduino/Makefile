MCU = atmega328p

EXE = usbtest.exe
HEX = software_usb.hex

CC = avr-g++
OBJCOPY = avr-objcopy

CFLAGS += -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections
CFLAGS += -fpack-struct -fshort-enums -g2 -Wall
CFLAGS += -mmcu=$(MCU)  -c -std=c++11 -MD -MP -MF

LDFLAGS += -Wl,-Map="memory_map.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections  -mmcu=$(MCU)

OBJFLAGS = -j .text -j .data -O ihex

OBJECTS = usbdrvasm.o usbdrv.o oddebug.o

all: $(EXE) $(HEX)

$(EXE): usbtest.cxx
	@echo
	g++ -std=c++1y -I ./libusb/include -O -Wall usbtest.cxx ./libusb/lib/dynamic/libusb_dyn.c -o $@

clean:
	$(RM)  *.d *.elf *.hex *.map *.o *.d *.o *.exe

%.hex: %.elf
	$(OBJCOPY) $(OBJFLAGS) $< $@
	@echo
	avr-size *.elf

software_usb.elf: $(OBJECTS)
	@echo
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	@echo
	$(CC) $(CFLAGS) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -c $< -o $@

%.o: %.cpp
	@echo
	$(CC) $(CFLAGS) "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -c $< -o $@

%.o: %.S
	@echo
	$(CC) -mmcu=$(MCU) -Wa,-gdwarf2 -x assembler-with-cpp -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -Wa,-g    $< -o $@
