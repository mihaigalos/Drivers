#ifndef __TMP_112_H_
#define __TMP_112_H_

#include <Wire.h>

constexpr uint8_t TMP102_CFG_default_byte1 = 0b01100001;  // 12 bit WITH ShutDown bit turned ON as well
constexpr uint8_t TMP102_CFG_default_byte2 = 0b10100000;  // just the defaults from pg 7
constexpr uint8_t TMP102_OneShotBit = 0b10000000;  // one-shot by ORing D7 of CFG byte 1 to 1

// 1-byte pointer to write to tmp102 BEFORE reading back 2 bytes of data from that register
constexpr uint8_t TMP102_TEMP_REG_pointer = 0x00;  // temperature register, read only, 16bits
constexpr uint8_t TMP102_CONF_REG_pointer = 0x01;  // config register, read/write, 16 bits

constexpr uint8_t TMP102_I2C_ADDR = 0x48;

class Tmp112{
public:
  Tmp112(uint8_t address = TMP102_I2C_ADDR) : address_{address}{}
  float getTemperature() {
    float result = 0;
    uint8_t errorflag=0;
    // start by resetting the one shot bit back to zero
    Wire.beginTransmission(address_);
    Wire.write(TMP102_CONF_REG_pointer);
    Wire.write(TMP102_CFG_default_byte1);  //Sets to 12bit, sd mode on
    errorflag = Wire.endTransmission();
    if ( errorflag != 0) {
      errorflag=0;
    }
    // now seting the one-shot bit to "1" will start a single conversion
    Wire.beginTransmission(address_);
    Wire.write(TMP102_CONF_REG_pointer); // point at the control register.
    Wire.write(TMP102_CFG_default_byte1 | TMP102_OneShotBit); // ORing the bits together
    //Wire.write(TMP102_CFG_default_byte2);  //I don't need to bother writing the second byte?
    errorflag = Wire.endTransmission();
    if ( errorflag != 0) {
      errorflag=0;
    }

    delay(28); // use wdt to sleep here?
    // wait for the conversion to happen: 26ms according to the sheet, will get zeros if I try to read the register too soon!
    // during the conversion the OS bit will temporarily read "0", then revert to "1" after the conversion
    // so you could check for that event

    Wire.beginTransmission(address_); //now read the temp
    Wire.write(TMP102_TEMP_REG_pointer); // Select temperature register.
    errorflag = Wire.endTransmission();
    if ( errorflag != 0) {
      errorflag=0;
    }
    Wire.requestFrom(static_cast<int>(address_), static_cast<int>(2));
    const uint8_t TempByte1 = Wire.read(); // MSByte, should be signed whole degrees C.
    const uint8_t TempByte2 = Wire.read(); // unsigned because I am not reading any negative temps
    const uint16_t Temp16 = (TempByte1 << 4) | (TempByte2 >> 4);    // builds 12-bit value
    result = static_cast<float>(Temp16)*0.0625;
    return result;
  }
private:
  uint8_t address_;
};
#endif //__TMP_112_H_
