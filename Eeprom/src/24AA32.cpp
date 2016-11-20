#include "application.h"
#include "e2prom.h"

using namespace std;

uint8_t E2PROM::ownAddress_;

E2PROM::E2PROM(uint8_t ownAddress, uint32_t speed){
    Wire.setSpeed(speed); 
    Wire.begin();
    E2PROM::ownAddress_ = ownAddress;
}


uint8_t E2PROM::readByte(uint16_t registerAddress)
{
    uint8_t data=0;
    
    Wire.beginTransmission(ownAddress_); 
    Wire.write(static_cast<uint8_t>(registerAddress>>8));
    Wire.write(static_cast<uint8_t>(registerAddress));
    Wire.endTransmission();
    
    //Ask the I2C device for data
    Wire.requestFrom(ownAddress_, 1);
    //while(!Wire.available());
    if(Wire.available()){
        data = Wire.read();
    }
    return data;
}



void E2PROM::dump(uint8_t columnCount){
    Serial.begin(9600);
    Serial.println("----------------------------------------");
    uint8_t buf [30]; memset (buf, 0, sizeof(buf));
    for (auto i = 0; i < 20; i++){
        Serial.print (readByte(i)); Serial.print (' ');
    }
    
    Serial.print('\n');
    Serial.end();
}

void E2PROM::writeByte(uint16_t registerAddress, uint8_t data){
    //Send the register address to be read.
    Wire.beginTransmission(ownAddress_);
    
    //Send the Register Address
    Wire.write(static_cast<uint8_t>(registerAddress>>8));
    Wire.write(static_cast<uint8_t>(registerAddress));
    
    Wire.write(data); 
    //End the communication sequence.
    Wire.endTransmission();
}

TEEepromResult  E2PROM::writePage(uint16_t registerAddress, uint8_t* buffer, uint8_t byteCount){
    TEEepromResult result = TEEepromResult_Ok;
    if(byteCount > 32) result = TEEepromResult_BufferGreaterAsPageSize;
    else {
        Wire.beginTransmission(ownAddress_);
        Wire.write(static_cast<uint8_t>(registerAddress>>8));
        Wire.write(static_cast<uint8_t>(registerAddress));
        for(auto i = 0; i<byteCount; i++){
            Wire.write(static_cast<char>(buffer[i]));
        }
        Wire.endTransmission();
    }
    return result;
}

TEEepromResult  E2PROM::readPage (uint16_t registerAddress, uint8_t* buffer, uint8_t byteCount){
    TEEepromResult result = TEEepromResult_Ok;
    if(byteCount > 32) result = TEEepromResult_BufferGreaterAsPageSize;
    else {
        Wire.beginTransmission(ownAddress_); 
        Wire.write(static_cast<uint8_t>(registerAddress>>8));
        Wire.write(static_cast<uint8_t>(registerAddress));
        Wire.endTransmission();
        
        Wire.requestFrom(ownAddress_, byteCount); 
        
        for(auto i = 0; i<byteCount; i++){
            buffer[i++] = Wire.read();
        }
    }
    Wire.reset();
    return result;
}
