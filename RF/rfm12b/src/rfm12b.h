// RFM12B driver definitions
// http://opensource.org/licenses/mit-license.php
// 2012-12-12 (C) felix@lowpowerlab.com
// Based on the RFM12 driver from jeelabs.com (2009-02-09 <jc@wippler.nl>)

#ifndef RFM12B_h
#define RFM12B_h
#include "config.h"
#ifndef COMPILE_FOR_AVR
#include "application.h"
#include <inttypes.h>
#else
#include <avr/io.h>

#define digitalWrite(A, B)                                                     \
  if (1 == B)                                                                  \
    PORTB |= (1 << A);                                                         \
  else                                                                         \
    PORTB &= ~(1 << A);
#define digitalRead(A) (PORTB & (1 << A))
#include <stdint.h>
#include <string.h> // for memcpy

typedef unsigned char byte;

#define OUTPUT 1
#define INPUT 0

#define pinMode(A, B)                                                          \
  if (1 == B)                                                                  \
    PORTB |= (1 << A);                                                         \
  else                                                                         \
    PORTB &= ~(0 << A);

#include <util/delay.h>
#define delay(A) _delay_ms(A);
#define delayMicroseconds(A) _delay_us(A);
#endif

#define TRANSMISSION_HEADER_LENGTH                                             \
  4 // <Network Id>    <Destination>     <Source>   <Length>
#define CRC_LENGTH_BYTES 2

/// RF12 Driver version
#define OPTIMIZE_SPI 1 // uncomment this to write to the RFM12B @ 8 Mhz

/// RF12 Maximum message size in bytes.
#define RF12_MAXDATA 128
/// Max transmit/receive buffer: 4 header + data + 2 crc bytes
#define RF_MAX (RF12_MAXDATA + 6)

// frequency bands
#define RF12_315MHZ 0
#define RF12_433MHZ 1
#define RF12_868MHZ 2
#define RF12_915MHZ 3

// Low batteery threshold (eg 2v25 = 2.25V)
#define RF12_2v25 0
#define RF12_2v55 3
#define RF12_2v65 4
#define RF12_2v75 5
#define RF12_3v05 8
#define RF12_3v15 9
#define RF12_3v25 10

#define RF12_HDR_IDMASK 0x7F
#define RF12_HDR_ACKCTLMASK 0x80
#define RF12_DESTID (rf12_hdr1 & RF12_HDR_IDMASK)
#define RF12_SOURCEID (rf12_hdr2 & RF12_HDR_IDMASK)

// shorthands to simplify sending out the proper ACK when requested
#define RF12_WANTS_ACK                                                         \
  ((rf12_hdr2 & RF12_HDR_ACKCTLMASK) && !(rf12_hdr1 & RF12_HDR_ACKCTLMASK))

// options for RF12_sleep()
#define RF12_SLEEP 0
#define RF12_WAKEUP -1

/// Shorthand for RF12 group byte in rf12_buf.
#define rf12_grp rf12_buf[0]
/// pointer to 1st header byte in rf12_buf (CTL + DESTINATIONID)
#define rf12_hdr1 rf12_buf[1]
/// pointer to 2nd header byte in rf12_buf (ACK + SOURCEID)
#define rf12_hdr2 rf12_buf[2]

/// Shorthand for RF12 length byte in rf12_buf.
#define rf12_len rf12_buf[3]
/// Shorthand for first RF12 data byte in rf12_buf.
#define rf12_data (rf12_buf + 4)

// pin change interrupts are currently only supported on ATmega328's
// #define PINCHG_IRQ 1    // uncomment this to use pin-change interrupts

// pins used for the RFM12B interface - yes, there *is* logic in this madness:
//  - leave RFM_IRQ set to the pin which corresponds with INT0, because the
//    current driver code will use attachInterrupt() to hook into that
//  - (new) you can now change RFM_IRQ, if you also enable PINCHG_IRQ - this
//    will switch to pin change interrupts instead of attach/detachInterrupt()
//  - use SS_DDR, SS_PORT, and SS_BIT to define the pin you will be using as
//    select pin for the RFM12B (you're free to set them to anything you like)
//  - please leave SPI_SS, SPI_MOSI, SPI_MISO, and SPI_SCK as is, i.e. pointing
//    to the hardware-supported SPI pins on the ATmega, *including* SPI_SS !
#ifndef COMPILE_FOR_AVR
#define BOARD_VDD DAC
#define BOARD_GND A5

#define RFM_IRQ A4
#define SPI_SS A0   // PB2, pin 16
#define SPI_MOSI A2 // PB3, pin 17
#define SPI_MISO A3 // PB4, pin 18
#define SPI_SCK A1  // PB5, pin 19
#else
#define RFM_IRQ 0
#define SPI_SS 1
#define SPI_MOSI 2
#define SPI_MISO 3
#define SPI_SCK 4
#endif

// RF12 command codes
#define RF_RECEIVER_ON 0x82D9
#define RF_XMITTER_ON 0x8239
#define RF_IDLE_MODE 0x8209
#define RF_SLEEP_MODE 0x8201
#define RF_WAKEUP_MODE 0x8201
#define RF_TXREG_WRITE 0xB800
#define RF_RX_FIFO_READ 0xB000
#define RF_WAKEUP_TIMER 0xE000

// RF12 status bits
#define RF_LBD_BIT 0x0400
#define RF_RSSI_BIT 0x0100

// transceiver states, these determine what to do with each interrupt
enum {
  TXCRC1,
  TXCRC2,
  TXTAIL,
  TXTAIL2,
  TXTAIL3,
  TXDONE,
  TXIDLE,
  TXRECV,
  TXPRE1,
  TXPRE2,
  TXPRE3,
  TXSYN1,
  TXSYN2,
};

#ifndef COMPILE_FOR_AVR
typedef enum {
  TELedState_Unknown,
  TELedState_Green,
  TELedState_RedBlue
} TELedState;
#endif

extern volatile uint8_t
    rf12_buf[RF_MAX]; // recv/xmit buf, including hdr & crc bytes
class RFM12B {
  static volatile uint8_t rxfill;    // number of data bytes in rf12_buf
  static volatile int8_t rxstate;    // current transceiver state
  static volatile uint16_t rf12_crc; // running crc value
  static uint32_t seqNum;            // encrypted send sequence number
  static uint32_t cryptKey[4];       // encryption key to use
  static long rf12_seq;              // seq number of encrypted packet (or -1)
  static uint8_t cs_pin;             // chip select pin
  void (*crypter)(bool);             // does en-/decryption (null if disabled)
  static uint8_t Byte(uint8_t out);
  static uint16_t XFER(uint16_t cmd);

  void SPIInit();

public:
  // constructor
  RFM12B() {}

  static uint8_t networkID; // network group
  static uint8_t ownId;     // address of this node
  static const byte DATAMAXLEN;

#ifndef COMPILE_FOR_AVR
  static TELedState ledState;
  static void LedToggle();
#endif

  static void TxRx();

  // Defaults: Group: 0x55, transmit power: 0(max), KBPS: 38.3Kbps (air
  // transmission baud - has to be same on all radios in same group)
  void Initialize(uint8_t ownId, uint8_t freqBand, uint8_t groupid = 0x55,
                  uint8_t txPower = 0, uint8_t airKbps = 0x08,
                  uint8_t lowVoltageThreshold = RF12_2v75);

  void ReceiveStart();
  bool ReceiveComplete();
  bool CanSend();
  uint16_t Control(uint16_t cmd);

  void SendStart(uint8_t toNodeId, bool requestACK = false,
                 bool sendACK = false);
  void SendStart(uint8_t toNodeId, const void *sendBuf, uint8_t sendLen,
                 bool requestACK = false, bool sendACK = false);
  void SendACK(const void *sendBuf = "", uint8_t sendLen = 0);
  void Send(uint8_t toNodeId, const void *sendBuf, uint8_t sendLen,
            bool requestACK = false);
  void SendWait();

  void OnOff(uint8_t value);
  void Sleep(char n);
  void Sleep();
  void Wakeup();

  volatile uint8_t *GetData();

  uint8_t GetSender();
  bool LowBattery();
  bool ACKRequested();
  bool ACKReceived(uint8_t fromNodeID = 0);

  bool CRCPass() { return rf12_crc == 0; }

#ifdef USE_ENCRYPTION
  static void CryptFunction(bool sending);
  void Encrypt(const uint8_t *key, uint8_t keyLen = 16);
#endif
};

#endif
