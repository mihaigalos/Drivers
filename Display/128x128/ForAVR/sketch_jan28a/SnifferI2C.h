#pragma once

#include "AbstractSniffer.h"

class SnifferI2C : public AbstractSniffer {
  uint8_t own_address_;
  static void onReceive(int count);

public:
  SnifferI2C(ObserverCallback observer, uint8_t own_address = 0)
      : own_address_(own_address) {
    AbstractSniffer::observer_ = observer;
  }
  void init();
  void run();
  void setOwnAddress(uint8_t address) { own_address_ = address; }
};
