#include <Arduino.h>
#include <Wire.h>

#include "tmp112.h"

constexpr uint8_t kConfig_12Bytes_shut_down = 0b01100001;
constexpr uint8_t kConfig_one_shot = 0b10000000;

constexpr uint8_t kTemperatureRegisterStartPointer = 0x00;
constexpr uint8_t kConfigurationRegisterStartPointer = 0x01;

constexpr float kSensorResolution = 0.0625;
constexpr int16_t kSensorResolution_bits = 12;

float Tmp112::getTemperature() {
  reset();
  enableSingleConversion();
  delay(28);
  return readTemperature();
}

void Tmp112::reset() {
  Wire.begin();
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
  const uint8_t temperature_msb = Wire.read();
  const uint8_t temperature_lsb = Wire.read();
  int16_t temperature = (temperature_msb << 4) | (temperature_lsb >> 4);
  if (temperature >= (1 << kSensorResolution_bits)) {
    temperature &= ~(1 << kSensorResolution_bits);
    temperature -= (1 << kSensorResolution_bits);
  }
  return static_cast<float>(temperature) * kSensorResolution;
}
