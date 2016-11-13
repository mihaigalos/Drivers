#pragma once

//#define DEBUG_

#ifdef DEBUG_
    #define dbg2(X,Y) Serial.print(X,Y)
    #define dbg(X) Serial.print(X)
#else
    #define dbg2(X,Y)
    #define dbg(X)
    
#endif