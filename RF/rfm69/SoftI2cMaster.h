/* Arduino DigitalIO Library
 * Copyright (C) 2013 by William Greiman
 *
 * This file is part of the Arduino DigitalIO Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino DigitalIO Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SOFT_I2C_MASTER_H
#define SOFT_I2C_MASTER_H
/**
 * @file
 * @brief Software I2C library
 *
 * @defgroup softI2C Software I2C
 * @details  Software Two Wire Interface library.
 * @{
 */
#if ARDUINO < 100
#error Requires Arduino 1.0 or greater.
#else  // ARDUINO
#include <Arduino.h>
#endif  // ARDUINO
#include <util/delay_basic.h>
#include <DigitalPin.h>
#include <I2cConstants.h>
//------------------------------------------------------------------------------
// State codes.

/** Stop condition transmitted. */
const uint8_t STATE_STOP = 0;

/** Repeated start condition transmitted. */
const uint8_t STATE_REP_START = 1;

/** Read data transfer active. */
const uint8_t STATE_RX_DATA = 2;

/** Write data transfer active. */
const uint8_t STATE_TX_DATA = 3;

/** Slave address plus read bit transmitted, NACK received. */
const uint8_t STATE_RX_ADDR_NACK = 4;

/** Slave address plus write bit transmitted, NACK received. */
const uint8_t STATE_TX_ADDR_NACK = 5;
/** Data byte transmitted, NACK received. */
const uint8_t STATE_TX_DATA_NACK = 6;
//==============================================================================
/**
 * @class I2cMasterBase
 * @brief Base class for FastI2cMaster, SoftI2cMaster
 */
class I2cMasterBase {
 public:
  I2cMasterBase() : _state(STATE_STOP) {}
  /** Read a byte
   *
   * @note This function should only be used by experts. Data should be
   *       accessed by calling transfer() and transferContinue()
   *
   * @param[in] last send a NACK to terminate read if last is true else
   *            send an ACK to continue the read.
   *
   * @return byte read from I2C bus
   */
  virtual uint8_t read(uint8_t last) = 0;

  /** Issue a start condition
   *
   * @note This function should only be used by experts. Data should be
   *       accessed by calling transfer() and transferContinue()
   */
  virtual void start() = 0;
  /** Issue a stop condition.
   *
   * @note This function should only be used by experts. Data should be
   *       accessed by calling transfer() and transferContinue()
   */
  virtual void stop() = 0;

  bool transfer(uint8_t addressRW, void *buf,
                size_t nbyte, uint8_t option = I2C_STOP);
                
  bool transferContinue(void *buf, size_t nbyte, uint8_t option = I2C_STOP);
  /** Write a byte
   *
   * @note This function should only be used by experts. Data should be
   *       accessed by calling transfer() and transferContinue()
   *
   * @param[in] data byte to write
   * @return true for ACK or false for NACK */
  virtual bool write(uint8_t data) = 0;

 private:
  uint8_t _state;
};
//==============================================================================
/**
 * @class SoftI2cMaster
 * @brief Software I2C master class
 */
class SoftI2cMaster : public I2cMasterBase {
 public:
  SoftI2cMaster() {}
  SoftI2cMaster(uint8_t sclPin, uint8_t sdaPin);
  void begin(uint8_t sclPin, uint8_t sdaPin);
  uint8_t read(uint8_t last);
  void start();
  void stop(void);
  bool write(uint8_t b);
  
 private:
  uint8_t _sclBit;
  uint8_t _sdaBit;
  volatile uint8_t* _sclDDR;
  volatile uint8_t* _sdaDDR;
  volatile uint8_t* _sdaInReg;
  //----------------------------------------------------------------------------
  bool readSda() {return *_sdaInReg & _sdaBit;}
  //----------------------------------------------------------------------------
  void sclDelay(uint8_t n) {_delay_loop_1(n);}
  //----------------------------------------------------------------------------
  bool writeScl(bool value) {
	  uint8_t s = SREG;
	  noInterrupts();
  	if (value == LOW) {
      // Pull scl low.
		  *_sclDDR |= _sclBit;
	  } else {
      // Put scl in high Z  input mode.
	  	*_sclDDR &= ~_sclBit;
  	}
	  SREG = s;
  }
  //----------------------------------------------------------------------------
  bool writeSda(bool value) {
	  uint8_t s = SREG;
	  noInterrupts();
	  if (value == LOW) {
      // Pull sda low.
	  	*_sdaDDR |= _sdaBit;
	  } else {
      // Put sda in high Z input mode.
	  	*_sdaDDR &= ~_sdaBit;
	  }
	  SREG = s;
  }
};
//==============================================================================
// Template based fast software I2C
//------------------------------------------------------------------------------
/**
 * @class FastI2cMaster
 * @brief Fast software I2C master class.
 */
template<uint8_t sclPin, uint8_t sdaPin>
class FastI2cMaster : public I2cMasterBase {
 public:
  //----------------------------------------------------------------------------
  FastI2cMaster() {
    begin();
  }
  //----------------------------------------------------------------------------
  /** Initialize I2C bus pins. */
  void begin() {
    fastDigitalWrite(sclPin, LOW);
    fastDigitalWrite(sdaPin, LOW);

    sclWrite(HIGH);
    sdaWrite(HIGH);
  }
  //----------------------------------------------------------------------------
  uint8_t read(uint8_t last) {
    uint8_t data = 0;
    sdaWrite(HIGH);

    readBit(7, &data);
    readBit(6, &data);
    readBit(5, &data);
    readBit(4, &data);
    readBit(3, &data);
    readBit(2, &data);
    readBit(1, &data);
    readBit(0, &data);

    // send ACK or NACK
    sdaWrite(last);
    sclDelay(4);
    sclWrite(HIGH);
    sclDelay(6);
    sclWrite(LOW);
    sdaWrite(LOW);
    return data;
  }
  //----------------------------------------------------------------------------
  void start() {
    if (!fastDigitalRead(sdaPin)) {
      // It's a repeat start.
      sdaWrite(HIGH);
      sclDelay(8);
      sclWrite(HIGH);
      sclDelay(8);
    }
    sdaWrite(LOW);
    sclDelay(8);
    sclWrite(LOW);
    sclDelay(8);
  }
  //----------------------------------------------------------------------------
  void stop(void) {
    sdaWrite(LOW);
    sclDelay(8);
    sclWrite(HIGH);
    sclDelay(8);
    sdaWrite(HIGH);
    sclDelay(8);
  }
  //----------------------------------------------------------------------------
  bool write(uint8_t data) {
    // write byte
    writeBit(7, data);
    writeBit(6, data);
    writeBit(5, data);
    writeBit(4, data);
    writeBit(3, data);
    writeBit(2, data);
    writeBit(1, data);
    writeBit(0, data);

    // get ACK or NACK
    sdaWrite(HIGH);

    sclWrite(HIGH);
    sclDelay(5);
    bool rtn = fastDigitalRead(sdaPin);
    sclWrite(LOW);
    sdaWrite(LOW);
    return rtn == 0;
  }
 private:
  //----------------------------------------------------------------------------
  inline __attribute__((always_inline))
  void sclWrite(bool value) {fastPinMode(sclPin, !value);}
  //----------------------------------------------------------------------------
  inline __attribute__((always_inline))
  void sdaWrite(bool value) {fastPinMode(sdaPin, !value);}
  //----------------------------------------------------------------------------
  inline __attribute__((always_inline))
  void readBit(uint8_t bit, uint8_t* data) {
    sclWrite(HIGH);
    sclDelay(5);
    if (fastDigitalRead(sdaPin)) *data |= 1 << bit;
    sclWrite(LOW);
    if (bit) sclDelay(6);
  }
  //----------------------------------------------------------------------------
  void sclDelay(uint8_t n) {_delay_loop_1(n);}
  //----------------------------------------------------------------------------
  inline __attribute__((always_inline))
  void writeBit(uint8_t bit, uint8_t data) {
    uint8_t mask = 1 << bit;
    sdaWrite(data & mask);
    sclWrite(HIGH);
    sclDelay(5);
    sclWrite(LOW);
    sclDelay(5);
  }
};
#endif  // SOFT_I2C_MASTER_H
/** @} */