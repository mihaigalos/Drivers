/**
 * Author: Mihai Galos, Joonas Pihlajamaa
 * Inspired by http://codeandlife.com
 * Inspired by V-USB example code by Christian Starkjohann
 * License: GNU GPL v3 (see License.txt)
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv.h"

#define F_CPU 16000000L
#include <util/delay.h>

#include <stdint.h>
#include <stdlib.h>

#include "i_usbRequest.h"
#include "usbdrv.h"
#include "software_usb.h"


extern "C" void USB_INTR_VECTOR(void);

uint8_t buffer[kBufferSize] {"Hello, USB!"};
uint8_t dataReceived, dataLength; // for USB_DATA_IN

[[noreturn]]
void reset_microcontroller(){
    void(*resetFunc) (void) = 0;
    resetFunc();
}

SoftwareUSB::SoftwareUSB (){
  dataReceived =0;
  dataLength=0;

//wdt_enable (WDTO_1S); // enable 1s watchdog timer
//	fillBufferFromFlash();
	usbInit();

	usbDeviceDisconnect(); // enforce re-enumeration
	for (uint8_t i = 0; i < 250; i++) { // wait 500 ms
		wdt_reset(); // keep the watchdog happy
		_delay_ms(2);
	}
	usbDeviceConnect();

	sei(); // Enable interrupts after re-enumeration
  
}

void SoftwareUSB::spin() {



		if (USB_INTR_PENDING & (1 << USB_INTR_PENDING_BIT)) {
			USB_INTR_VECTOR();
			USB_INTR_PENDING = 1 << USB_INTR_PENDING_BIT; // Clear int pending, in case timeout occured during SYNC                     
		}

		wdt_reset(); // keep the watchdog happy
		usbPoll();

}

void SoftwareUSB::fillBufferFromFlash(uint16_t offset = 0) {
	for (uint16_t i = 0; i < sizeof(buffer); ++i) {
		buffer[i] = pgm_read_byte_near(i + offset);
	}
	offset += sizeof(buffer);
}

// this gets called when custom control message is received
USB_PUBLIC uint8_t SoftwareUSB::usbFunctionSetup(uint8_t data[8]) {
	usbRequest_t *rq = reinterpret_cast<usbRequest_t *>(data); // cast data to correct type

	switch (static_cast<USBRequest>(rq->bRequest)) { // custom command is in the bRequest field
	case USBRequest::LED_ON:
    DDRD  |= (1 << 1);
    PORTD &= ~(1 << 1); // turn LED on
		return 0;
	case USBRequest::LED_OFF:
    DDRD  |= (1 << 1);
    PORTD |= (1 << 1); // turn LED off
		return 0;
	case USBRequest::DATA_OUT: // send data to PC
		//fillBufferFromFlash();
		usbMsgPtr = buffer;
		return sizeof(buffer);
	case USBRequest::DATA_WRITE: // modify reply buffer
	case USBRequest::FLASH_DUMP_FROM_ADDRESS: // receive data from PC
		dataLength = (uint8_t) rq->wLength.word;
		dataReceived = 0;

		if (dataLength > sizeof(buffer)) // limit to buffer size
			dataLength = sizeof(buffer);

		return USB_NO_MSG; // usbFunctionWrite will be called now
	}

	return 0; // should not get here
}

void SoftwareUSB::handleFunctionWrite() {

	int startOffset = (int) strtol(reinterpret_cast<const char*>(&buffer[0]),
			NULL, 16);

	fillBufferFromFlash(startOffset);

}

uint8_t* SoftwareUSB::getBuffer() {
  return &buffer[0];
}

uint8_t SoftwareUSB::getBufferLength() {
  return dataLength;
}

// This gets called when data is sent from PC to the device
USB_PUBLIC uint8_t SoftwareUSB::usbFunctionWrite(uint8_t *data, uint8_t len) {
	uint8_t i;
	for (i = 0; dataReceived < dataLength && i < len; i++, dataReceived++)
		buffer[dataReceived] = data[i];

	handleFunctionWrite();

	return (dataReceived == dataLength); // 1 if we received it all, 0 if not
}

