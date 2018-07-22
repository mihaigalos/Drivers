#pragma once

#include <stdint.h>

enum class ButtonMenuState{
  Red,
  Green,
  Blue,
  Magenta,
  Cyan,
  Yellow,
  White,
  EndOfList
};

ButtonMenuState currentButtonMenuState=ButtonMenuState::White;

ButtonMenuState& operator++(ButtonMenuState& s){
  using IntType = uint8_t;//TODO: For C++14: typename std::underlying_type<ButtonMenuState>::type
  s = static_cast<ButtonMenuState>( static_cast<IntType>(s) + 1 );
  if ( s == ButtonMenuState::EndOfList )
    s = static_cast<ButtonMenuState>(0);
  return s;
}

inline void ledOn(uint8_t led){
  digitalWrite(led, LOW);
}

inline void ledOff(uint8_t led){
  digitalWrite(led, HIGH);
}

class IColor{
public:
  explicit IColor(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin):
  red_pin_(red_pin), green_pin_(green_pin), blue_pin_(blue_pin){}
  virtual ~IColor(){}
  virtual void onEnter() =0;
  virtual void onExit(){
    ledOff(red_pin_);
    ledOff(green_pin_);
    ledOff(blue_pin_);
  }
protected:
  uint8_t red_pin_;
  uint8_t green_pin_;
  uint8_t blue_pin_;
};

class RedColor : public IColor{
public:
  RedColor(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin):IColor(red_pin, green_pin, blue_pin){}
  void onEnter() override {
    onExit();
    ledOn(red_pin_);
  }
};

class GreenColor : public IColor{
public:
  GreenColor(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin):IColor(red_pin, green_pin, blue_pin){}
  virtual void onEnter() override {
    onExit();
    ledOn(green_pin_);
  }
};

class BlueColor : public IColor{
public:
  BlueColor(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin):IColor(red_pin, green_pin, blue_pin){}
  virtual void onEnter() override {
    onExit();
    ledOn(blue_pin_);
  }
};

class MagentaColor : public IColor{
public:
  MagentaColor(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin):IColor(red_pin, green_pin, blue_pin){}
  virtual void onEnter() override {
    onExit();
    ledOn(red_pin_);
    ledOn(blue_pin_);
  }
};

class CyanColor : public IColor{
public:
  CyanColor(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin):IColor(red_pin, green_pin, blue_pin){}
  virtual void onEnter() override {
    onExit();
    ledOn(green_pin_);
    ledOn(blue_pin_);
  }
};

class YellowColor : public IColor{
public:
  YellowColor(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin):IColor(red_pin, green_pin, blue_pin){}
  virtual void onEnter() override {
    onExit();
    ledOn(red_pin_);
    ledOn(green_pin_);
  }
};

class WhiteColor : public IColor{
public:
  WhiteColor(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin):IColor(red_pin, green_pin, blue_pin){}
  virtual void onEnter() override {
    onExit();
    ledOn(red_pin_);
    ledOn(green_pin_);
    ledOn(blue_pin_);
  }
};

class ButtonMenu{
public:

  static ButtonMenuState get() { return currentButtonMenuState; }

  static void changeState(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin){
    ++currentButtonMenuState;
    switch(currentButtonMenuState){
      case ButtonMenuState::Red:
        RedColor{red_pin, green_pin, blue_pin}.onEnter();
      break;

      case ButtonMenuState::Green:
      GreenColor{red_pin, green_pin, blue_pin}.onEnter();
      break;

      case ButtonMenuState::Blue:
        BlueColor{red_pin, green_pin, blue_pin}.onEnter();
      break;

      case ButtonMenuState::Magenta:
        MagentaColor{red_pin, green_pin, blue_pin}.onEnter();
      break;

      case ButtonMenuState::Cyan:
        CyanColor{red_pin, green_pin, blue_pin}.onEnter();
      break;

      case ButtonMenuState::Yellow:
        YellowColor{red_pin, green_pin, blue_pin}.onEnter();
      break;

      case ButtonMenuState::White:
        WhiteColor{red_pin, green_pin, blue_pin}.onEnter();
      break;

      default:
      WhiteColor{red_pin, green_pin, blue_pin}.onEnter();
    }
  }
};
