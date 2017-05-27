#include <DigitalIO.h>
#include <DigitalPin.h>
#include <I2cConstants.h>
#include <PinIO.h>
#include <RFM69.h>
#include <RFM69registers.h>
#include <SoftI2cMaster.h>
#include <SoftSPI.h>

//#define SEND
#define RECEIVE

#ifdef SEND
  #define OWNID 0x10
  #define TOID     0x01  //the node ID we're sending to
#else 
  #define OWNID 0x01
#endif

RFM69 rf;

void myCallbackHandler(){
  //print message received to serial
    Serial.print('[');Serial.print(rf.SENDERID);Serial.print("] ");
    Serial.print((char*)rf.DATA);
    Serial.print("   [RX_RSSI:");Serial.print(rf.RSSI);Serial.print("]");
    Serial.println();

    if (rf.ACKRequested())
    {
          rf.sendACK();
          Serial.print(" - ACK sent");
    }
  
}

void setup() {
   
  // put your setup code here, to run once:
  rf.initialize(RF69_868MHZ, OWNID,0xFF); // TODO: reenable node address in library
  rf.promiscuous(true);
  
  #ifdef RECEIVE
  rf.setCallback(myCallbackHandler);
  #endif
  
  Serial.begin(115200);
  rf.readAllRegs();

  
}

void loop() {
  
  #ifdef SEND
      //if(rf.canSend())
      {
        char buf [] = "abcdefghijklmnopqrstuvwxyzABCDEF";
        rf.send(TOID, &buf, strlen(buf));
        //rf.sendWithRetry(TOID, "ACK TEST", 8, 250); // 0 = only 1 attempt, no retries
      }
      pinMode(8, OUTPUT); digitalWrite(8, !digitalRead(8)); 
  #elif defined (RECEIVE) // RECEIVE
      rf.receiveDone();      
  #endif // SEND or RECEIVE

  delay(1000);
}


