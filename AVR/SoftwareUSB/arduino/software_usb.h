#pragma once

#include <stdint.h>

#ifndef USB_PUBLIC
  #define USB_PUBLIC
#endif

using TFunc_void_puint8_uint8 = void(*)(uint8_t*, uint8_t);
// static inline uint8_t readByte(const uint8_t source_address,const uint16_t register_address)

typedef struct {
    uint8_t is_dumping_flash:1;
    uint8_t is_callback_perform:1;
    uint8_t is_dumping_eeprom:1;
}TStateFlags;

class SoftwareUSB{
public:
  SoftwareUSB ();
  void spin();
  uint8_t* getBuffer();
  uint8_t getBufferLength();

  static uint16_t getStartOffset();

  // this gets called when custom control message is received
  static USB_PUBLIC uint8_t usbFunctionSetup(uint8_t data[8]);

  static inline void fillBufferFromFlash(uint16_t offset);
  static inline void fillBufferFromEeprom(uint16_t offset);
  // This gets called when data is sent from PC to the device
  static USB_PUBLIC uint8_t usbFunctionWrite(uint8_t *data, uint8_t len);

  static void copyToUSBBuffer(uint8_t *data, uint8_t len);

  static TFunc_void_puint8_uint8 callback_on_usb_data_receive_;
  // static bool is_dumping_flash_, is_callback_perform_, is_dumping_eeprom_;
  static TStateFlags state_flags_;
};
