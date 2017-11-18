#include "SnifferI2C.h"
#include <Wire.h>

void SnifferI2C::init() {
    AbstractSniffer::reset();
}

void SnifferI2C::run() {
  Wire.begin(own_address_);
  Wire.onReceive(onReceive);
}

void SnifferI2C::onReceive(int count) {
  while (0 < Wire.available()) { // loop through all but the last
    if(captured_data_count_ < CAPTURE_SIZE){
      captured_data_[captured_data_count_++] = Wire.read(); 
    } else { // report
      observer_();
    }
  }
}
