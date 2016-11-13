#include "RFM12B.h"

//#define SEND

#define OWNID 0x12
String payload = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~!@#$%^&*(){}[]`|<>?+=:;,.";
#define TOID     1  //the node ID we're sending to
RFM12B r;
void setup() {
    r.Initialize(OWNID, RF12_868MHZ);
}

void loop() {
    
    #ifdef SEND
        r.Wakeup();
        auto len = payload.length()+1;
        char buf [255];
        payload.toCharArray(buf, len);
        r.Send(TOID, buf, len);
        delay (3000);
    #else 
        r.Wakeup();
        r.ReceiveStart();
        r.Sleep();
    #endif
}