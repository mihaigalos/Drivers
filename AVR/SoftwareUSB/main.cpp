/**
 * Author: Mihai Galos, Joonas Pihlajamaa
 * Inspired by http://codeandlife.com
 * Inspired by V-USB example code by Christian Starkjohann
 * License: GNU GPL v3 (see License.txt)
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv/usbdrv.h"

#define F_CPU 16000000L
#include <util/delay.h>

#include <stdint.h>
#include <stdlib.h>

#include "i_usbRequest.h"

static uchar buffer[kBufferSize] = "Hello, USB!";
static uchar dataReceived = 0, dataLength = 0; // for USB_DATA_IN

void fillBufferFromFlash(uint16_t offset = 0) {
	for (uint16_t i = 0; i < sizeof(buffer); ++i) {
		buffer[i] = pgm_read_byte_near(i + offset);
	}
	offset += sizeof(buffer);
}

// this gets called when custom control message is received
USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = reinterpret_cast<usbRequest_t *>(data); // cast data to correct type

	switch (static_cast<USBRequest>(rq->bRequest)) { // custom command is in the bRequest field
	case USBRequest::LED_ON:
		PORTD &= ~(1 << 1); // turn LED on
		return 0;
	case USBRequest::LED_OFF:
		PORTD |= (1 << 1); // turn LED off
		return 0;
	case USBRequest::DATA_OUT: // send data to PC
		//fillBufferFromFlash();
		usbMsgPtr = buffer;
		return sizeof(buffer);
	case USBRequest::DATA_WRITE: // modify reply buffer
		buffer[7] = rq->wValue.bytes[0];
		buffer[8] = rq->wValue.bytes[1];
		buffer[9] = rq->wIndex.bytes[0];
		buffer[10] = rq->wIndex.bytes[1];
		return 0;

	case USBRequest::FLASH_DUMP_FROM_ADDRESS: // receive data from PC

		dataLength = (uchar) rq->wLength.word;
		dataReceived = 0;

		if (dataLength > sizeof(buffer)) // limit to buffer size
			dataLength = sizeof(buffer);

		return USB_NO_MSG; // usbFunctionWrite will be called now
	}

	return 0; // should not get here
}

static inline void handleFunctionWrite() {
	int startOffset = (int) strtol(reinterpret_cast<const char*>(&buffer[0]),
			NULL, 16);

	fillBufferFromFlash(startOffset);

}

// This gets called when data is sent from PC to the device
USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len) {
	uchar i;
	for (i = 0; dataReceived < dataLength && i < len; i++, dataReceived++)
		buffer[dataReceived] = data[i];

	handleFunctionWrite();

	return (dataReceived == dataLength); // 1 if we received it all, 0 if not
}

extern "C" void USB_INTR_VECTOR(void);

int main() {
	uchar i;

	DDRD = (1 << 0 | 1 << 1); // PB0 as output
	PORTD = 0xFE;
//wdt_enable (WDTO_1S); // enable 1s watchdog timer
//	fillBufferFromFlash();
	usbInit();

	usbDeviceDisconnect(); // enforce re-enumeration
	for (i = 0; i < 250; i++) { // wait 500 ms
		wdt_reset(); // keep the watchdog happy
		_delay_ms(2);
	}
	usbDeviceConnect();

	sei(); // Enable interrupts after re-enumeration

	while (1) {
		if (USB_INTR_PENDING & (1 << USB_INTR_PENDING_BIT)) {
			USB_INTR_VECTOR();
			USB_INTR_PENDING = 1 << USB_INTR_PENDING_BIT; // Clear int pending, in case timeout occured during SYNC                     
		}

		wdt_reset(); // keep the watchdog happy
		usbPoll();
	}

	return 0;
}
