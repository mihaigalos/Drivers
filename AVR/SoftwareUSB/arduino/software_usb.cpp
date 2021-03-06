/**
 * Author: Mihai Galos, Joonas Pihlajamaa
 * Inspired by http://codeandlife.com
 * Inspired by V-USB example code by Christian Starkjohann
 * License: GNU GPL v3 (see License.txt)
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include "usbdrv.h"

#define F_CPU 16000000L
#include <util/delay.h>

#include <stdint.h>
#include <stdlib.h>

#include "i_usbRequest.h"
#include "usbdrv.h"
#include "software_usb.h"

#include "Arduino.h"

extern "C" void USB_INTR_VECTOR(void);


static constexpr uint8_t kI2CMaximumTransmissionSize = 32; // fixed from Arduino
uint8_t buffer[kBufferSize] {"Hello, USB!"};
uint8_t dataReceived, dataLength; // for USB_DATA_IN

TFunc_void_puint8_uint8 SoftwareUSB::callback_on_usb_data_receive_;
TFunc_uint8_constUint8_constUint16_constUint8_uint8P SoftwareUSB::handler_i2c_read_;
TFunc_uint8_constUint8_constUint16_constUint8 SoftwareUSB::handler_i2c_write_;
TStateFlags SoftwareUSB::state_flags_;

[[noreturn]]
void reset_microcontroller(){
    void(*resetFunc) (void) = 0;
    resetFunc();
}

SoftwareUSB::SoftwareUSB (){
  dataReceived =0;
  dataLength=0;
  callback_on_usb_data_receive_ = nullptr;
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
  if(SoftwareUSB::state_flags_.is_callback_perform){
    state_flags_.is_callback_perform= false;
    callback_on_usb_data_receive_(buffer, dataLength);
  }
}

void SoftwareUSB::fillBufferFromFlash(uint16_t offset) {
	for (uint16_t i = 0; i < sizeof(buffer); ++i) {
		buffer[i] = pgm_read_byte_near(i + offset);
	}
}

void SoftwareUSB::fillBufferFromEeprom(uint16_t offset) {
  for (uint16_t i = 0; i < sizeof(buffer); ++i) {
		buffer[i] = eeprom_read_byte(reinterpret_cast<uint8_t *>(i + offset));
	}
}

// this gets called when custom control message is received
USB_PUBLIC uint8_t SoftwareUSB::usbFunctionSetup(uint8_t data[8]) {
	usbRequest_t *rq = reinterpret_cast<usbRequest_t *>(data); // cast data to correct type
  state_flags_.is_dumping_flash = false;
  state_flags_.is_callback_perform = false;
  state_flags_.is_dumping_eeprom = false;
  state_flags_.is_read_i2c = false;

  state_flags_.is_dumping_i2c = false;
  state_flags_.is_write_i2c = false;


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
		usbMsgPtr = buffer;
		return sizeof(buffer);
  case USBRequest::RESET:
    reset_microcontroller();
    return 0;
	case USBRequest::DATA_WRITE:
    goto data_continue;

  case USBRequest::I2C_WIRE_DUMP:
    state_flags_.is_dumping_i2c = true;
    goto data_continue;
  case USBRequest::I2C_WIRE_READ:
    state_flags_.is_read_i2c = true;
    goto data_continue;
  case USBRequest::I2C_WIRE_WRITE:
    state_flags_.is_write_i2c = true;
    goto data_continue;
  case USBRequest::EEPROM_DUMP_FROM_ADDRESS:
    state_flags_.is_dumping_eeprom = true;
    goto data_continue;
  case USBRequest::FLASH_DUMP_FROM_ADDRESS: // receive data from PC
    state_flags_.is_dumping_flash = true;
    data_continue:
    dataLength = static_cast<uint8_t>(rq->wLength.word);
		dataReceived = 0;

		if (dataLength > sizeof(buffer)) // limit to buffer size
			dataLength = sizeof(buffer);

		return USB_NO_MSG; // usbFunctionWrite will be called now
	}

	return 0; // should not get here
}

uint16_t SoftwareUSB::getStartOffset() {
	return static_cast<uint16_t>(strtol(reinterpret_cast<const char*>(&buffer[0]),
			NULL, 16));
}


// This gets called when data is sent from PC to the device
USB_PUBLIC uint8_t SoftwareUSB::usbFunctionWrite(uint8_t *data, uint8_t len) {
  uint8_t i;
	for (i = 0; dataReceived < dataLength && i < len; i++, dataReceived++)
		buffer[dataReceived] = data[i];

  bool is_fully_received = dataReceived == dataLength;

  if(state_flags_.is_dumping_flash){
    fillBufferFromFlash(getStartOffset());
  }else if(state_flags_.is_dumping_eeprom){
    fillBufferFromEeprom(getStartOffset());
  }else if(state_flags_.is_write_i2c){
    if(handler_i2c_write_){
      uint8_t position_space1 = String(reinterpret_cast<char*>(buffer)).indexOf(" ");
      uint8_t position_space2 = String(reinterpret_cast<char*>(buffer)).indexOf(" ",position_space1+1);

      String device_address_substring(reinterpret_cast<char*>(buffer));
      device_address_substring = device_address_substring.substring(0, position_space1);
      uint8_t device_address = strtol(device_address_substring.c_str(), 0, 16);

      String register_address_substring(reinterpret_cast<char*>(buffer));
      register_address_substring= register_address_substring.substring(position_space1+1, position_space2);
      uint8_t register_address = strtol(register_address_substring.c_str(), 0, 16);

      String value_substring(reinterpret_cast<char*>(buffer));
      value_substring = value_substring.substring(position_space2+1);
      uint8_t value = strtol(value_substring.c_str(), 0, 16);

      uint8_t result = handler_i2c_write_(device_address, register_address, value);
      if(0 == result){
        buffer[0] = 'O'; buffer[1] = 'K'; buffer[2]='\0';
      } else {
        strncpy(buffer, "I2C Error ", 10);

        buffer[10] = '0'+result;
        buffer[11] = '\0';
      }
    }
  }else if(state_flags_.is_read_i2c){
    if(handler_i2c_read_){
      uint8_t position_space = String(reinterpret_cast<char*>(buffer)).indexOf(" ");

      String device_address_substring(reinterpret_cast<char*>(buffer));
      device_address_substring = device_address_substring.substring(0, position_space);
      uint8_t device_address = strtol(device_address_substring.c_str(), 0, 16);

      String register_address_substring(reinterpret_cast<char*>(buffer));
      register_address_substring= register_address_substring.substring(position_space+1);
      uint8_t register_address = strtol(register_address_substring.c_str(), 0, 16);

      handler_i2c_read_(device_address, register_address,kI2CMaximumTransmissionSize,&buffer[0]);
    }
  }else if(state_flags_.is_dumping_i2c){

    if(handler_i2c_read_){
      uint8_t device_address= strtol(reinterpret_cast<char*>(buffer), 0, 16);
      for(uint16_t i = 0; i< kBufferSize;i+=kI2CMaximumTransmissionSize){
        handler_i2c_read_(device_address, i, kI2CMaximumTransmissionSize, &buffer[i]);
      }
    }
  } else if(is_fully_received && nullptr != callback_on_usb_data_receive_){
    state_flags_.is_callback_perform = true;
  }

	return is_fully_received; // 1 if we received it all, 0 if not
}

void SoftwareUSB::copyToUSBBuffer(uint8_t *data, uint8_t len){
  for(uint8_t i = 0; i<len && i<kBufferSize;++i){
    buffer[i] = data[i];
  }
}
