/**
 * Project: AVR ATtiny USB Tutorial at http://codeandlife.com/
 * Author: Joonas Pihlajamaa, joonas.pihlajamaa@iki.fi
 * Inspired by V-USB example code by Christian Starkjohann
 * Copyright: (C) 2012 by Joonas Pihlajamaa
 * License: GNU GPL v3 (see License.txt)
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv/usbdrv.h"

#define F_CPU 16000000L
#include <util/delay.h>

#include <stdint.h>

#include "i_usbRequest.h"


#define columnCount 31
#define bytesPercolumn 8

static uchar replyBuf[bytesPercolumn * columnCount] = "Hello, USB!";
static uchar dataReceived = 0, dataLength = 0; // for USB_DATA_IN

void fillBufferFromFlash() {
	uint16_t offset = 0;
	for (uint16_t i = 0; i < sizeof(replyBuf); ++i) {
		replyBuf[i] = pgm_read_byte_near(i + offset);
	}
	offset += sizeof(replyBuf);

}

// this gets called when custom control message is received
USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = reinterpret_cast<usbRequest_t *> (data); // cast data to correct type

	switch (static_cast<USBRequest>(rq->bRequest)) { // custom command is in the bRequest field
	case USBRequest::USB_LED_ON:
		PORTD &= ~(1 << 0) & ~(1 << 1); // turn LED on
		return 0;
	case USBRequest::USB_LED_OFF:
		PORTD |= (1 << 0) | (1 << 1); // turn LED off
		return 0;
	case USBRequest::USB_DATA_OUT: // send data to PC
		//fillBufferFromFlash();
		usbMsgPtr = replyBuf;
		return sizeof(replyBuf);
	case USBRequest::USB_DATA_WRITE: // modify reply buffer
		replyBuf[7] = rq->wValue.bytes[0];
		replyBuf[8] = rq->wValue.bytes[1];
		replyBuf[9] = rq->wIndex.bytes[0];
		replyBuf[10] = rq->wIndex.bytes[1];
		return 0;
	case USBRequest::USB_DATA_IN: // receive data from PC
		dataLength = (uchar) rq->wLength.word;
		dataReceived = 0;

		if (dataLength > sizeof(replyBuf)) // limit to buffer size
			dataLength = sizeof(replyBuf);

		return USB_NO_MSG; // usbFunctionWrite will be called now
	}

	return 0; // should not get here
}

// This gets called when data is sent from PC to the device
USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len) {
	uchar i;

	for (i = 0; dataReceived < dataLength && i < len; i++, dataReceived++)
		replyBuf[dataReceived] = data[i];

	return (dataReceived == dataLength); // 1 if we received it all, 0 if not
}

extern "C" void USB_INTR_VECTOR(void);

int main() {
	uchar i;

	DDRD = (1 << 1 | 1 << 0); // PB0 as output

//wdt_enable (WDTO_1S); // enable 1s watchdog timer
	fillBufferFromFlash();
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
