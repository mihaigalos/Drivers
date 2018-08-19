#include <stdint.h>

class Supercapacitor{
public:
  Supercapacitor(const uint8_t& red_led, const uint8_t& green_led, const uint8_t& blue_led) : red_led_(red_led), green_led_(green_led), blue_led_(blue_led){}
  virtual void voltageToLeds() = 0;
  void flashLed(const uint8_t led, const uint8_t times) const;
protected:
  const uint8_t &red_led_;
  const uint8_t &green_led_;
  const uint8_t &blue_led_;

};

class Supercapacitor3V : public Supercapacitor{
public:
  Supercapacitor3V(const uint8_t& red_led, const uint8_t& green_led, const uint8_t& blue_led) : Supercapacitor(red_led, green_led, blue_led){}
  void voltageToLeds();
};

class Supercapacitor2_7V : public Supercapacitor{
public:
  Supercapacitor2_7V(const uint8_t& red_led, const uint8_t& green_led, const uint8_t& blue_led) : Supercapacitor(red_led, green_led, blue_led){}
  void voltageToLeds();
};
