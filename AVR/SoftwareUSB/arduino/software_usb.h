#pragma once

#include <stdint.h>

#ifndef USB_PUBLIC
  #define USB_PUBLIC
#endif
  

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


};