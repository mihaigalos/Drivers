#ifndef __TMP_112_H_
#define __TMP_112_H_

constexpr uint8_t tmp112_i2c_default_address = 0x48;

class Tmp112 {
public:
  Tmp112(uint8_t address = tmp112_i2c_default_address) : address_{address} {}
  float getTemperature();
  void shutDown();

private:
  void reset();
  void enableSingleConversion();
  float readTemperature();

  uint8_t address_;
};
#endif //__TMP_112_H_
