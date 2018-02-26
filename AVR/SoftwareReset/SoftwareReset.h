#pragma once

#include <PinChangeInt.h>

using TVoidVoid = void(*)();

void softReset(){
    asm("jmp 0");
}

class SoftwareReset{

public:
  SoftwareReset(TVoidVoid callback=softReset, int in_pin = 3) : callback_{callback}, in_pin_{in_pin} {setup_();}

private:
  int in_pin_ {3};  
  
  TVoidVoid callback_;
  
  void setup_(){
    pinMode(in_pin_,INPUT_PULLUP);
    attachPinChangeInterrupt(in_pin_, *callback_, FALLING);
  }
};