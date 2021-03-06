MCU = atmega328p

EXE = usbterminal.exe
HEX = software_usb.hex

CC = avr-g++
OBJCOPY = avr-objcopy

CFLAGS += -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections
CFLAGS += -fpack-struct -fshort-enums -g2 -Wall
CFLAGS += -mmcu=$(MCU)  -c -std=c++11 -MD -MP -MF

LDFLAGS += -Wl,-Map="memory_map.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections  -mmcu=$(MCU)

OBJFLAGS = -j .text -j .data -O ihex

OBJECTS = usbdrvasm.o usbdrv.o oddebug.o

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
  COMPILE_COMMAND = g++ -std=c++11 -O -Wall usbterminal.cxx -o usbterminal -lusb
else
  COMPILE_COMMAND = g++ -std=c++11 -I ./libusb/include -O -Wall usbterminal.cxx ./libusb/lib/dynamic/libusb_dyn.c -o $@
endif


all: $(EXE) $(HEX)

$(EXE): usbterminal.cxx
	@echo
	$(COMPILE_COMMAND)

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
