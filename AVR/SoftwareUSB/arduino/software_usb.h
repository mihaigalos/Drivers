#pragma once

#include "i_usbRequest.h"
#include "usbdrv.h"

extern uchar buffer[kBufferSize];
extern uchar dataReceived, dataLength; // for USB_DATA_IN

class SoftwareUSB{

public:

  SoftwareUSB ();
  void spin();

private:

  void fillBufferFromFlash(uint16_t offset = 0);

  // this gets called when custom control message is received
  USB_PUBLIC uchar usbFunctionSetup(uchar data[8]);

  inline void handleFunctionWrite();

  // This gets called when data is sent from PC to the device
  USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len);


};