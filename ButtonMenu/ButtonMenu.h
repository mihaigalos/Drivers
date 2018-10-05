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
  TRXCyan,
  TRXMagenta,
  EndOfList
};

ButtonMenuState currentButtonMenuState=static_cast<ButtonMenuState>(static_cast<uint8_t>(ButtonMenuState::EndOfList)-1);

ButtonMenuState& operator++(ButtonMenuState& s){
  using IntType = uint8_t;//TODO: For C++14: typename std::underlying_type<ButtonMenuState>::type
  s = static_cast<ButtonMenuState>( static_cast<IntType>(s) + 1 );
  if ( s == ButtonMenuState::EndOfList )
    s = static_cast<ButtonMenuState>(0);
  return s;
}

inline void pinLow(uint8_t led){
  digitalWrite(led, LOW);
}

inline void pinHigh(uint8_t led){
  digitalWrite(led, HIGH);
}

inline void ledHiImpedance(uint8_t led){
  pinMode(led, INPUT);
}

class IColor{
public:
  explicit IColor(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin):
  red_pin_{red_pin}, green_pin_{green_pin}, blue_pin_{blue_pin}, trx_pin_{trx_pin}{}
  virtual ~IColor(){}
  virtual void onEnter() =0;
  virtual void onExit(){
    pinHigh(red_pin_);
    pinHigh(green_pin_);
    pinHigh(blue_pin_);
    ledHiImpedance(trx_pin_);
  }
protected:
  uint8_t red_pin_;
  uint8_t green_pin_;
  uint8_t blue_pin_;
  uint8_t trx_pin_;
};

class Red : public IColor{
public:
  Red(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin):IColor{red_pin, green_pin, blue_pin, trx_pin}{}
  void onEnter() override {
    onExit();
    pinLow(red_pin_);
  }
};

class Green : public IColor{
public:
  Green(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin):IColor{red_pin, green_pin, blue_pin, trx_pin}{}
  virtual void onEnter() override {
    onExit();
    pinLow(green_pin_);
  }
};

class Blue : public IColor{
public:
  Blue(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin):IColor{red_pin, green_pin, blue_pin, trx_pin}{}
  virtual void onEnter() override {
    onExit();
    pinLow(blue_pin_);
  }
};

class Magenta : public IColor{
public:
  Magenta(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin):IColor{red_pin, green_pin, blue_pin, trx_pin}{}
  virtual void onEnter() override {
    onExit();
    pinLow(red_pin_);
    pinLow(blue_pin_);
  }
};

class Cyan : public IColor{
public:
  Cyan(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin):IColor{red_pin, green_pin, blue_pin, trx_pin}{}
  virtual void onEnter() override {
    onExit();
    pinLow(green_pin_);
    pinLow(blue_pin_);
  }
};

class Yellow : public IColor{
public:
  Yellow(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin):IColor{red_pin, green_pin, blue_pin, trx_pin}{}
  virtual void onEnter() override {
    onExit();
    pinLow(red_pin_);
    pinLow(green_pin_);
  }
};

class White : public IColor{
public:
  White(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin):IColor{red_pin, green_pin, blue_pin, trx_pin}{}
  virtual void onEnter() override {
    onExit();
    pinLow(red_pin_);
    pinLow(green_pin_);
    pinLow(blue_pin_);
  }
};

class TRXCyan : public IColor{
public:
  TRXCyan(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin):IColor{red_pin, green_pin, blue_pin, trx_pin}{}
  virtual void onEnter() override {
    onExit();
    pinMode(trx_pin_, OUTPUT);
    pinHigh(trx_pin_);
  }
};

class TRXMagenta : public IColor{
public:
  TRXMagenta(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin):IColor{red_pin, green_pin, blue_pin, trx_pin}{}
  virtual void onEnter() override {
    onExit();
    pinMode(trx_pin_, OUTPUT);
    pinLow(trx_pin_);
  }
};

class ButtonMenu{
public:

  static ButtonMenuState get() { return currentButtonMenuState; }

  static void changeState(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t trx_pin){
    ++currentButtonMenuState;
    switch(currentButtonMenuState){
      case ButtonMenuState::Red:
        Red{red_pin, green_pin, blue_pin, trx_pin}.onEnter();
      break;

      case ButtonMenuState::Green:
      Green{red_pin, green_pin, blue_pin, trx_pin}.onEnter();
      break;

      case ButtonMenuState::Blue:
        Blue{red_pin, green_pin, blue_pin, trx_pin}.onEnter();
      break;

      case ButtonMenuState::Magenta:
        Magenta{red_pin, green_pin, blue_pin, trx_pin}.onEnter();
      break;

      case ButtonMenuState::Cyan:
        Cyan{red_pin, green_pin, blue_pin, trx_pin}.onEnter();
      break;

      case ButtonMenuState::Yellow:
        Yellow{red_pin, green_pin, blue_pin, trx_pin}.onEnter();
      break;

      case ButtonMenuState::White:
        White{red_pin, green_pin, blue_pin, trx_pin}.onEnter();
      break;

      case ButtonMenuState::TRXCyan:
        TRXCyan{red_pin, green_pin, blue_pin, trx_pin}.onEnter();
      break;

      case ButtonMenuState::TRXMagenta:
        TRXMagenta{red_pin, green_pin, blue_pin, trx_pin}.onEnter();
      break;

      default:
      White{red_pin, green_pin, blue_pin, trx_pin}.onEnter();
    }
  }
};
