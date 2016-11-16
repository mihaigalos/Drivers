/*
 * Rfm12b.cpp
 *
 * Created: 11/16/2016 8:21:58 PM
 * Author : Mihai Galos
 */ 

#include "rfm12b.h"
#define OWNID 0x12
#define TOID     1  //the node ID we're sending to

int main(void)
{
	RFM12B r;
	r.Initialize(OWNID, RF12_868MHZ);
    /* Replace with your application code */
    while (1) 
    {
		// receive
		r.Wakeup();
		r.ReceiveStart();
		r.Sleep();

		// Send
		r.Wakeup();

		char buf [] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~!@#$%^&*(){}[]`|<>?+=:;,.";

		r.Send(TOID, buf, strlen(buf));
		delay (500);
    }
}

