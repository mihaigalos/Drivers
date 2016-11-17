// RFM12B driver definitions
// http://opensource.org/licenses/mit-license.php
// 2012-12-12 (C) felix@lowpowerlab.com
// Based on the RFM12 driver from jeelabs.com (2009-02-09 <jc@wippler.nl>)

#include "RFM12B.h"

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

uint8_t RFM12B::cs_pin;                // CS pin for SPI
uint8_t RFM12B::ownId;                // address of this node
uint8_t RFM12B::networkID;             // network group ID
long RFM12B::rf12_seq;
uint32_t RFM12B::seqNum;
uint32_t RFM12B::cryptKey[4];
volatile uint8_t RFM12B::rxfill;       // number of data bytes in rf12_buf
volatile int8_t RFM12B::rxstate;       // current transceiver state
volatile uint16_t RFM12B::rf12_crc;    // running crc value
volatile uint8_t rf12_buf[RF_MAX];     // recv/xmit buf, including hdr & crc bytes

#ifndef COMPILE_FOR_AVR
TELedState RFM12B::ledState;
#endif

uint16_t
_crc16_update(uint16_t crc, uint8_t a)
{
    int i;

    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }
    return crc;
}

void RFM12B::SPIInit() {
	 delay(100); // wait for chrystal
 #ifdef COMPILE_FOR_AVR  
	
 #else
	pinMode(BOARD_VDD, OUTPUT);
	digitalWrite(BOARD_VDD,HIGH);
	pinMode(BOARD_GND, OUTPUT);
	digitalWrite(BOARD_GND, LOW);
#endif
    
	pinMode(SPI_SS, OUTPUT);
	digitalWrite(SPI_SS, 1);
	
	pinMode(SPI_MOSI, OUTPUT);
	pinMode(SPI_MISO, INPUT);
	pinMode(SPI_SCK, OUTPUT);
	pinMode(RFM_IRQ, INPUT);
  
}

uint8_t RFM12B::Byte(uint8_t out) {
   
    uint8_t result = 0;
    for(int i = 7; i>=0; i--){
        uint8_t c = 0;
        if(((1<<i) & (out))) {
            c = 1;
        }
        digitalWrite(SPI_MOSI, c);
        delayMicroseconds(5);
        digitalWrite(SPI_SCK, 1);
        delayMicroseconds(5);
        result |= digitalRead(SPI_MISO)<<i;
        digitalWrite(SPI_SCK, 0);
        

    }
    digitalWrite(SPI_MOSI, 0);
    return result;
}


uint16_t RFM12B::XFER(uint16_t cmd) {
    uint16_t result =0;
    digitalWrite(SPI_SS, 0);
    result =Byte(cmd >> 8)<<8;
    result|=Byte(cmd);
    digitalWrite(SPI_SS, 1);
    return result;
}

// Call this once with params:
// - node ID (0-31)
// - frequency band (RF12_433MHZ, RF12_868MHZ, RF12_915MHZ)
// - networkid [optional - default = 170] (0-255 for RF12B, only 212 allowed for RF12)
// - txPower [optional - default = 0 (max)] (7 is min value)
// - AirKbps [optional - default = 38.31Kbps]
// - lowVoltageThreshold [optional - default = RF12_2v75]
void RFM12B::Initialize(uint8_t ID, uint8_t freqBand, uint8_t networkid, uint8_t txPower, uint8_t airKbps, uint8_t lowVoltageThreshold)
{
    
    #ifdef DEBUG_
        Serial.begin();
    #endif
    
	#ifndef COMPILE_FOR_AVR
		ledState = TELedState_Green;
		RGB.control(true);
	#endif
    
    ownId = ID;
    networkID = networkid;
    SPIInit();
    
    XFER(0x0000); // intitial SPI transfer added to avoid power-up problem
    XFER(RF_SLEEP_MODE); // DC (disable clk pin), enable lbd
    
    // wait until RFM12B is out of power-up reset, this takes several *seconds*
    XFER(RF_TXREG_WRITE); // in case we're still in OOK mode
    while (digitalRead(RFM_IRQ) == 0) XFER(0x0000);
    
    XFER(0x80C7 | (freqBand << 4)); // EL (ena TX), EF (ena RX FIFO), 12.0pF
    XFER(0x8209);               // enable xtal, disable clk pin
    XFER(0xA640); // Frequency is exactly 434/868/915MHz (whatever freqBand is)
    XFER(0xC680 + airKbps);   //Air transmission baud rate
    XFER(0x94A0);             // VDI,FAST,134kHz,0dBm,-103dBm
    XFER(0xC2AC);             // AL,!ml,DIG,DQD4
    if (networkID != 0) {
        XFER(0xCA83);           // FIFO8,2-SYNC,!ff,DR
        XFER(0xCE00 | networkID); // SYNC=2DXX;
    } else {
        XFER(0xCA8B); // FIFO8,1-SYNC,!ff,DR
        XFER(0xCE2D); // SYNC=2D;
    }
    XFER(0xC483); // @PWR,NO RSTRIC,!st,!fi,OE,EN
    
    XFER(0x9850 | (txPower > 7 ? 7 : txPower)); // !mp,90kHz,MAX OUT               //last byte=power level: 0=highest, 7=lowest
    XFER(0xCC77); // OB1,OB0, LPX,!ddy,DDIT,BW0
    XFER(0xE000); // NOT USE
    XFER(0xC800); // NOT USE
    XFER(0xC040); // Clock output (1.66MHz), Low Voltage threshold (2.55V)
    
    rxstate = TXIDLE;
     
}

uint16_t RFM12B::Control(uint16_t cmd) {
  uint16_t r = XFER(cmd);
    return r;
}

#ifndef COMPILE_FOR_AVR
void RFM12B::LedToggle(){
    switch(ledState){
        case TELedState_Green: 
            RGB.color(0,255,0);
            ledState = TELedState_RedBlue;
        break;
        case TELedState_RedBlue:
            RGB.color(255,0,255);
            ledState = TELedState_Green;
        break;
    }
}
#endif
void RFM12B::TxRx() {
  rf12_crc = 0;
  if (rxstate == TXRECV) {
      
        uint16_t length = 255, receivedCrc = 0;
        rxfill = 0; 
        dbg("Receiving.\n\r\n\r");
        do{
            
            while (   (1 == digitalRead(RFM_IRQ)) );
               
            XFER(0x0000); 
            uint8_t in = XFER(RF_RX_FIFO_READ);
           
            if(rxfill < (length - CRC_LENGTH_BYTES)){
                rf12_buf[rxfill] = in;
                rf12_crc = _crc16_update(rf12_crc, in);
            }
            rxfill++;
            dbg2(in, HEX); dbg(' '); dbg("      CRC: "); dbg2(rf12_crc>>8, HEX); dbg("_"); dbg2((uint8_t)rf12_crc, HEX);
            if ( (rxfill == 0) && (networkID != 0)){
                //rf12_buf[rxfill] = networkID;  
            }
            else if(TRANSMISSION_HEADER_LENGTH == rxfill){ // length is within the header
               
                length = in + TRANSMISSION_HEADER_LENGTH + CRC_LENGTH_BYTES;
                dbg(String("Got Length: ")+String(length) + String("   "));
            }
            else if (rxfill == (length - CRC_LENGTH_BYTES+1)){
                receivedCrc = in; dbg(" --- CRC1");
            } else if (rxfill == length ){
                receivedCrc |= (in << 8); dbg(" --- CRC2");
            }
            
            dbg("\n\r");
        } while( rxfill < length);
      
    dbg("Got CRC: "); dbg2((uint8_t)(receivedCrc>>8), HEX); dbg("_"); dbg2((uint8_t)receivedCrc, HEX) ;dbg ("   ");
    dbg("Computed CRC: "); dbg2((uint8_t)(rf12_crc>>8), HEX); dbg("_"); dbg2((uint8_t)rf12_crc, HEX) ;dbg ("   ");
       
    #ifndef COMPILE_FOR_AVR
		if(receivedCrc == rf12_crc) LedToggle();
	#endif
        
  } else {
        XFER(0x0000);
        uint8_t out;
        dbg("Sending.\n\r\n\r");
        while (TXDONE != rxstate){
            // AA AA AA (pramble)   2D  <Network Id>    <Destination>     <Source>   <Length>     <Payload>    <CRC1>    <CRC2>
            
            while (1 == digitalRead(RFM_IRQ)); // wait for previous transmission to finish
            
            if (rxstate < 0) { // sending payload
                uint8_t pos = 4 + rf12_len + rxstate++;
                out = rf12_buf[pos];
                rf12_crc = _crc16_update(rf12_crc, out);
            } else
            switch (rxstate++) {
              case TXSYN1: out = 0x2D; break;
              case TXSYN2: out = networkID; rxstate = -(3 + rf12_len); rf12_crc = 0; break;
              case TXCRC1: out = rf12_crc;  dbg("CRC1: "); break;
              case TXCRC2: out = rf12_crc >> 8; dbg("CRC2: "); break;//dbg("\n\r"); break; 
              case TXDONE: XFER(RF_IDLE_MODE); dbg('\n');// fall through
              default:     out = 0xAA;
            }
            
            dbg2(out, HEX); dbg(' '); dbg("      CRC: "); dbg2(rf12_crc>>8, HEX); dbg("_"); dbg2((uint8_t)rf12_crc, HEX); dbg("\r");
            XFER(RF_TXREG_WRITE + out);
        } 
    }
    rxfill = 0; rf12_crc = 0;
    rxstate = TXDONE;
}

void RFM12B::ReceiveStart() {
    XFER(0xCA83); //Enable FIFO     
    
    rxfill = rf12_len = 0;
    rf12_crc = ~0;
    if (networkID != 0) rf12_crc = _crc16_update(~0, networkID);

    rxstate = TXRECV;
    XFER(RF_RECEIVER_ON);   
    TxRx();
    delay(10);
}

bool RFM12B::ReceiveComplete() {
  if (rxstate == TXRECV && (rxfill >= rf12_len + 6 || rxfill >= RF_MAX)) {
    rxstate = TXIDLE;
    if (rf12_len > RF12_MAXDATA)
      rf12_crc = 1; // force bad crc if packet length is invalid
    if (RF12_DESTID == 0 || RF12_DESTID == ownId) { //if (!(rf12_hdr & RF12_HDR_DST) || (nodeID & NODE_ID) == 31 || (rf12_hdr & RF12_HDR_MASK) == (nodeID & NODE_ID)) {
      if (rf12_crc == 0 && crypter != 0)
        crypter(false);
      else
        rf12_seq = -1;
      return true; // it's a broadcast packet or it's addressed to this node
    }
  }
  if (rxstate == TXIDLE)
    ReceiveStart();
  return false;
}

bool RFM12B::CanSend() {
  // no need to test with interrupts disabled: state TXRECV is only reached
  // outside of ISR and we don't care if rxfill jumps from 0 to 1 here
  
  uint16_t status = XFER(0000);
  if( (status & RF_RSSI_BIT ) == 0) {
    XFER(RF_IDLE_MODE); // stop receiver
    //XXX just in case, don't know whether these RF12 reads are needed!
    // rf12_XFER(0x0000); // status register
    // rf12_XFER(RF_RX_FIFO_READ); // fifo read
    rxstate = TXIDLE;
    return true;
  }
  return false;
}

void RFM12B::SendStart(uint8_t toNodeID, bool requestACK, bool sendACK) {
  rf12_hdr1 = toNodeID | (sendACK ? RF12_HDR_ACKCTLMASK : 0);
  rf12_hdr2 = ownId | (requestACK ? RF12_HDR_ACKCTLMASK : 0);
  if (crypter != 0) crypter(true);
  rf12_crc = ~0;
  rf12_crc = _crc16_update(rf12_crc, networkID);
  rxstate = TXPRE1;
  
  XFER(RF_XMITTER_ON); // bytes will be fed via interrupts
}

void RFM12B::SendStart(uint8_t toNodeID, const void* sendBuf, uint8_t sendLen, bool requestACK, bool sendACK) {

  rf12_len = sendLen;
  memcpy((void*) rf12_data, sendBuf, sendLen);
  SendStart(toNodeID, requestACK, sendACK);
  SendWait();
}

/// Should be called immediately after reception in case sender wants ACK
void RFM12B::SendACK(const void* sendBuf, uint8_t sendLen) {
  while (!CanSend()) delay(1000);
  SendStart(RF12_SOURCEID, sendBuf, sendLen, false, true);
}

void RFM12B::Send(uint8_t toNodeID, const void* sendBuf, uint8_t sendLen, bool requestACK)
{
  while (!CanSend());
  SendStart(toNodeID, sendBuf, sendLen, requestACK, false);
  Sleep();
}

void RFM12B::SendWait() {
    TxRx(); // manual call to send
}

void RFM12B::OnOff(uint8_t value) {
  XFER(value ? RF_XMITTER_ON : RF_IDLE_MODE);
}

void RFM12B::Sleep(char n) {
  if (n == 0){
    Control(RF_IDLE_MODE);
   
  }
  else {
    Control(RF_SLEEP_MODE);
  }
  rxstate = TXIDLE;
}
void RFM12B::Sleep() { Sleep(1); }
void RFM12B::Wakeup() { Sleep(0); }

bool RFM12B::LowBattery() {
  return (Control(0x0000) & RF_LBD_BIT) != 0;
}

uint8_t RFM12B::GetSender(){
  return RF12_SOURCEID;
}

volatile uint8_t * RFM12B::GetData() { return rf12_data; }

bool RFM12B::ACKRequested() { return RF12_WANTS_ACK; }

/// Should be polled immediately after sending a packet with ACK request
bool RFM12B::ACKReceived(uint8_t fromNodeID) {
  if (ReceiveComplete())
    return CRCPass() &&
           RF12_DESTID == ownId &&
          (RF12_SOURCEID == fromNodeID || fromNodeID == 0) &&
          (rf12_hdr1 & RF12_HDR_ACKCTLMASK) &&
          !(rf12_hdr2 & RF12_HDR_ACKCTLMASK);
  return false;
}

//--------------------------------------------------- ENCRYPTION --------------------------------------------------- 

#ifdef USE_ENCRYPTION

// XXTEA by David Wheeler, adapted from http://en.wikipedia.org/wiki/XXTEA
#define DELTA 0x9E3779B9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (cryptKey[(uint8_t)((p&3)^e)] ^ z)))
void RFM12B::CryptFunction(bool sending) {
  uint32_t y, z, sum, *v = (uint32_t*) rf12_data;
  uint8_t p, e, rounds = 6;

  if (sending) {
    // pad with 1..4-byte sequence number
    *(uint32_t*)(rf12_data + rf12_len) = ++seqNum;
    uint8_t pad = 3 - (rf12_len & 3);
    rf12_len += pad;
    rf12_data[rf12_len] &= 0x3F;
    rf12_data[rf12_len] |= pad << 6;
    ++rf12_len;
    // actual encoding
    char n = rf12_len / 4;
    if (n > 1) {
      sum = 0;
      z = v[n-1];
      do {
        sum += DELTA;
        e = (sum >> 2) & 3;
        for (p=0; p<n-1; p++)
            y = v[p+1], z = v[p] += MX;
        y = v[0];
        z = v[n-1] += MX;
      } while (--rounds);
    }
  } else if (rf12_crc == 0) {
    // actual decoding
    char n = rf12_len / 4;
    if (n > 1) {
      sum = rounds*DELTA;
      y = v[0];
      do {
        e = (sum >> 2) & 3;
        for (p=n-1; p>0; p--)
          z = v[p-1], y = v[p] -= MX;
        z = v[n-1];
        y = v[0] -= MX;
      } while ((sum -= DELTA) != 0);
    }
    // strip sequence number from the end again
    if (n > 0) {
      uint8_t pad = rf12_data[--rf12_len] >> 6;
      rf12_seq = rf12_data[rf12_len] & 0x3F;
      while (pad-- > 0)
        rf12_seq = (rf12_seq << 8) | rf12_data[--rf12_len];
    }
  }
}

void RFM12B::Encrypt(const uint8_t* key, uint8_t keyLen) {
  // by using a pointer to CryptFunction, we only link it in when actually used
  if (key != 0) {
    for (uint8_t i = 0; i < keyLen; ++i)
      ((uint8_t*) cryptKey)[i] = key[i];
    crypter = CryptFunction;
  } else crypter = 0;
}
#endif //#ifdef USE_ENCRYPTION