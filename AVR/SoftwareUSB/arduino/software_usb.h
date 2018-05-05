#pragma once

#include <stdint.h>

#ifndef USB_PUBLIC
  #define USB_PUBLIC
#endif

typedef void(*TFunc_void_puint8_uint8)(uint8_t*, uint8_t);

class SoftwareUSB{
public:
  SoftwareUSB ();
  void spin();
  uint8_t* getBuffer();
  uint8_t getBufferLength();

  static void fillBufferFromFlash(uint16_t offset = 0);

  // this gets called when custom control message is received
  static USB_PUBLIC uint8_t usbFunctionSetup(uint8_t data[8]);

  static inline void handleFunctionWrite();

  // This gets called when data is sent from PC to the device
  static USB_PUBLIC uint8_t usbFunctionWrite(uint8_t *data, uint8_t len);
  
  static TFunc_void_puint8_uint8 callback_on_usb_data_receive_;
  static bool is_dumping_flash_;
};