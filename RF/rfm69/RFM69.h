/*
        Copyright (C) 2015 Grzegorz Miasko <gieemek@wp.pl>

        based on RFM69 driver definition by Felix Rusu (2014),
   felix@lowpowerlab.com (protected by GNU General Public License:
   http://www.gnu.org/licenses/gpl-3.0.txt)

        This program is free software; you can redistribute it and/or
        modify it under the terms of the GNU General Public License
        version 3 as published by the Free Software Foundation.

        Please maintain this license information along with authorship
        and copyright notices in any redistribution of this code
*/

#ifndef RFM69_h
#define RFM69_h
#include <Arduino.h>

#include <DigitalIO.h>
#include <SoftSPI.h>

#define MIHAI_NO_WAIT_RECEIVE

// Dot v>1.3
const uint8_t SOFT_SPI_MISO_PIN = 6; // choose your own MISO pin if you wish
const uint8_t SOFT_SPI_MOSI_PIN = 5; // choose your own MOSI pin if you wish
const uint8_t SOFT_SPI_SCK_PIN = 4;  // choose your own SCL pin if you wish
#define RF69_SPI_CS 17

/*
// Dot v0.3
const uint8_t SOFT_SPI_MISO_PIN = 6;	// choose your own MISO pin if you wish
const uint8_t SOFT_SPI_MOSI_PIN = 5;	// choose your own MOSI pin if you wish
const uint8_t SOFT_SPI_SCK_PIN  = 4;	// choose your own SCL pin if you wish
#define RF69_SPI_CS 3
#define RF69_IRQ_PIN          17
#define RF69_IRQ_NUM          0
*/

const uint8_t SPI_MODE = 0;

// choose your own DATA pin if you wish
#define RF69_MAX_DATA_LEN                                                      \
  61 // to take advantage of the built in AES/CRC we want to limit the frame
     // size to the internal FIFO size (66 bytes - 3 bytes overhead - 2 bytes
     // crc)

// INT0 on AVRs should be connected to RFM69's DIO0 (ex on ATmega328 it's D2, on
// ATmega644/1284 it's D2)
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) ||               \
    defined(__AVR_ATmega88) || defined(__AVR_ATmega8__) ||                     \
    defined(__AVR_ATmega88__)
#define RF69_IRQ_PIN 2
#define RF69_IRQ_NUM 0
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
#define RF69_IRQ_PIN 2
#define RF69_IRQ_NUM 2
#elif defined(__AVR_ATmega32U4__)
#define RF69_IRQ_PIN 3
#define RF69_IRQ_NUM 0
#else
#define RF69_IRQ_PIN 2
#define RF69_IRQ_NUM 0
#endif

#define CSMA_LIMIT                                                             \
  -90 // upper RX signal sensitivity threshold in dBm for carrier sense access
#define RF69_MODE_SLEEP 0   // XTAL OFF
#define RF69_MODE_STANDBY 1 // XTAL ON
#define RF69_MODE_SYNTH 2   // PLL ON
#define RF69_MODE_RX 3      // RX MODE
#define RF69_MODE_TX 4      // TX MODE

// available frequency bands
#define RF69_315MHZ 31 // non trivial values to avoid misconfiguration
#define RF69_433MHZ 43
#define RF69_868MHZ 86
#define RF69_915MHZ 91

#define null 0
#define COURSE_TEMP_COEF                                                       \
  -90 // puts the temperature reading in the ballpark, user can fine tune the
      // returned value
#define RF69_BROADCAST_ADDR 255
#define RF69_CSMA_LIMIT_MS 1000
#define RF69_TX_LIMIT_MS 1000
#define RF69_FSTEP                                                             \
  61.03515625 // == FXOSC / 2^19 = 32MHz / 2^19 (p13 in datasheet)

typedef void (*TCallback_void_void)(void);

class RFM69 {
public:
  static volatile uint8_t
      DATA[RF69_MAX_DATA_LEN]; // recv/xmit buf, including header & crc bytes
  static volatile uint8_t DATALEN;
  static volatile uint8_t SENDERID;
  static volatile uint8_t TARGETID; // should match _address
  static volatile uint8_t PAYLOADLEN;
  static volatile uint8_t ACK_REQUESTED;
  static volatile uint8_t ACK_RECEIVED; // should be polled immediately after
                                        // sending a packet with ACK request
  static volatile int16_t
      RSSI; // most accurate RSSI during reception (closest to the reception)
  static volatile uint8_t _mode; // should be protected?

  SoftSPI<SOFT_SPI_MISO_PIN, SOFT_SPI_MOSI_PIN, SOFT_SPI_SCK_PIN, SPI_MODE> spi;

  RFM69(uint8_t slaveSelectPin = RF69_SPI_CS,
        uint8_t interruptPin = RF69_IRQ_PIN, bool isRFM69HW = false,
        uint8_t interruptNum = RF69_IRQ_NUM) {
    _slaveSelectPin = slaveSelectPin;
    _interruptPin = interruptPin;
    _interruptNum = interruptNum;
    _mode = RF69_MODE_STANDBY;
    _promiscuousMode = false;
    _powerLevel = 31;
    _isRFM69HW = isRFM69HW;
    _callback = NULL;
  }

  bool initialize(uint8_t freqBand, uint8_t ID, uint8_t networkID = 1);
  void setAddress(uint8_t addr);
  void setCallback(TCallback_void_void
                       callback); // observer callback in case packet received
  void setNetwork(uint8_t networkID);
  bool canSend();
  void send(uint8_t toAddress, const void *buffer, uint8_t bufferSize,
            bool requestACK = false);
  bool sendWithRetry(
      uint8_t toAddress, const void *buffer, uint8_t bufferSize,
      uint8_t retries = 2,
      uint8_t retryWaitTime = 40); // 40ms roundtrip req for 61byte packets
  bool receiveDone();
  bool ACKReceived(uint8_t fromNodeID);
  bool ACKRequested();
  void sendACK(const void *buffer = "", uint8_t bufferSize = 0);
  uint32_t getFrequency();
  void setFrequency(uint32_t freqHz);
  void encrypt(const char *key);
  void setCS(uint8_t newSPISlaveSelect);
  int16_t readRSSI(bool forceTrigger = false);
  void promiscuous(bool onOff = true);
  void setHighPower(
      bool onOFF = true); // has to be called after initialize() for RFM69HW
  void setPowerLevel(uint8_t level); // reduce/increase transmit power level
  void sleep();
  uint8_t readTemperature(uint8_t calFactor = 0); // get CMOS temperature (8bit)
  void rcCalibration(); // calibrate the internal RC oscillator for use in wide
                        // temperature variations - see datasheet section
                        // [4.3.5. RC Timer Accuracy]

  // allow hacking registers by making these public
  uint8_t readReg(uint8_t addr);
  void writeReg(uint8_t addr, uint8_t val);
  void readAllRegs();

protected:
  static void isr0();
  void virtual interruptHandler();
  void sendFrame(uint8_t toAddress, const void *buffer, uint8_t size,
                 bool requestACK = false, bool sendACK = false);

  static RFM69 *selfPointer;
  uint8_t _slaveSelectPin;
  uint8_t _interruptPin;
  uint8_t _interruptNum;
  uint8_t _address;
  bool _promiscuousMode;
  uint8_t _powerLevel;
  bool _isRFM69HW;
  uint8_t _SPCR;
  uint8_t _SPSR;

  TCallback_void_void _callback;

  void receiveBegin();
  void setMode(uint8_t mode);
  void setHighPowerRegs(bool onOff);
  void select();
  void unselect();
};

#endif
