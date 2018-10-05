#ifndef __TMP_112_H_
#define __TMP_112_H_

#include <Wire.h>

constexpr uint8_t kTmp112ConfigurationValue_12Bytes_shut_down = 0b01100001;  // 12 bit WITH ShutDown bit turned ON as well
constexpr uint8_t kTmp112ConfigurationValue_one_shot = 0b10000000;  // one-shot by ORing D7 of CFG byte 1 to 1

// 1-byte pointer to write to tmp102 BEFORE reading back 2 bytes of data from that register
constexpr uint8_t kTmp112TemperatureRegisterStartPointer = 0x00;
constexpr uint8_t kTmp112ConfigurationRegisterStartPointer = 0x01;

constexpr uint8_t tmp112_i2c_default_address = 0x48;

constexpr float tmp112_resolution = 0.0625;

class Tmp112{
public:
  Tmp112(uint8_t address = tmp112_i2c_default_address) : address_{address}{}

  float getTemperature() {
    enableSingleConversion();
    delay(28);
    return readTemperature();
  }
private:

  void reset(){
    Wire.beginTransmission(address_);
    Wire.write(kTmp112ConfigurationRegisterStartPointer);
    Wire.write(kTmp112ConfigurationValue_12Bytes_shut_down);
  }

  void enableSingleConversion(){
    Wire.beginTransmission(address_);
    Wire.write(kTmp112ConfigurationRegisterStartPointer);
    Wire.write(kTmp112ConfigurationValue_12Bytes_shut_down | kTmp112ConfigurationValue_one_shot);
    Wire.endTransmission();
  }

  float readTemperature(){
    float result = 0.0f;
    Wire.beginTransmission(address_); //now read the temp
    Wire.write(kTmp112TemperatureRegisterStartPointer); // Select temperature register.
    Wire.requestFrom(static_cast<int>(address_), static_cast<int>(2));
    const uint8_t TempByte1 = Wire.read(); // MSByte, should be signed whole degrees C.
    const uint8_t TempByte2 = Wire.read(); // unsigned because I am not reading any negative temps
    const uint16_t Temp16 = (TempByte1 << 4) | (TempByte2 >> 4);    // builds 12-bit value
    result = static_cast<float>(Temp16)*tmp112_resolution;
    return result;
  }

  uint8_t address_;
};
#endif //__TMP_112_H_
