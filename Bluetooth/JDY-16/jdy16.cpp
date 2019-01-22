#include "jdy16.h"

void JDY16::powerOnRequest(bool on) {
  pinMode(pin_, OUTPUT);

  if (on) {
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, HIGH);
    delay(50 / (1 << clockPrescaler_));
    digitalWrite(pin_, LOW);
    delay(50 / (1 << clockPrescaler_));
  } else {
    pinMode(pin_, INPUT);
  }
}

void JDY16::deepSleepNoBroadcast() {
  powerOnRequest(false);
  pointer_write_i2c_n_bytes_(0x50, 0x13, reinterpret_cast<const char *>(2), 1);
}

void JDY16::wakeUp() {
  powerOnRequest(true);
  pointer_write_i2c_n_bytes_(0x50, 0x13, reinterpret_cast<const char *>(0), 1);
}

void JDY16::readFromApp() {
  uint8_t app_data_length = pointer_read_data_length_(0x50);
  if (app_data_length > 0) {
    pointer_read_i2c_bytes_(0x50, 0xF2, app_data_length);
  }
}

void JDY16::writeToApp() {
  pointer_write_i2c_n_bytes_(0x50, 0xF0, "21.5   49%", 12);
}
