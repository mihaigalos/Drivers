#include <Arduino.h>
#include <Wire.h>

#include "tmp112.h"

constexpr uint8_t kConfig_12Bytes_shut_down = 0b01100001; // 12 bit + ShutDown
constexpr uint8_t kConfig_one_shot = 0b10000000;

constexpr uint8_t kTemperatureRegisterStartPointer = 0x00;
constexpr uint8_t kConfigurationRegisterStartPointer = 0x01;

constexpr float kSensorResolution = 0.0625;

float Tmp112::getTemperature() {
  reset();
  enableSingleConversion();
  delay(28);
  return readTemperature();
}

void Tmp112::reset() {
  Wire.begin(); // join I2C bus as master
  Wire.beginTransmission(address_);
  Wire.write(kConfigurationRegisterStartPointer);
  Wire.write(kConfig_12Bytes_shut_down);
  Wire.endTransmission();
}

void Tmp112::enableSingleConversion() {
  Wire.beginTransmission(address_);
  Wire.write(kConfigurationRegisterStartPointer);
  Wire.write(kConfig_12Bytes_shut_down | kConfig_one_shot);
  Wire.endTransmission();
}

float Tmp112::readTemperature() {
  Wire.beginTransmission(address_);
  Wire.write(kTemperatureRegisterStartPointer);
  Wire.endTransmission();

  Wire.requestFrom(static_cast<int>(address_), static_cast<int>(2));
  const uint8_t TempByte1 =
      Wire.read(); // MSByte, should be signed whole degrees C.
  const uint8_t TempByte2 =
      Wire.read(); // unsigned because I am not reading any negative temps
  const uint16_t Temp16 =
      (TempByte1 << 4) | (TempByte2 >> 4); // builds 12-bit value

  return static_cast<float>(Temp16) * kSensorResolution;
}
