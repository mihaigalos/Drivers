/**********************************************************
Software I2C Library for AVR.

Manually calibrated for common frequencies.
Copyright Mihai Galos 2017
**********************************************************/

#include <avr/io.h>

#include <string.h> // for memset
#include "i2c.h"

#define SPACING_DELAY ; //nothing per default, otherwise: _delay_us(1); 
#define HIGH_HOLD_DELAY _delay_us((1000.0f/(float)(SPEED_KHZ)) / 2 + MANUAL_CLK_OFFSET_USEC); // /2 because a period has HIGH and LOW component, 

void I2C::Begin()
{
    SDADDR |= (1<<SDA);
    SCLDDR |= (1<<SCL);

    SDA_HIGH;    
    SCL_HIGH;    
}
void I2C::Start()
{
    SCL_HIGH;
    HIGH_HOLD_DELAY;
    
    SDA_LOW;    
    HIGH_HOLD_DELAY;      
}

void I2C::Stop()
{
     SDA_LOW;
     HIGH_HOLD_DELAY;
     SCL_HIGH;
     HIGH_HOLD_DELAY;
     SDA_HIGH;
     HIGH_HOLD_DELAY;
}

bool I2C::WriteByte(uint8_t data)
{
     uint8_t i;
         
     for(i=0;i<8;i++)
     {
        SCL_LOW;
        SPACING_DELAY;
        
        if(data & 0x80)
            SDA_HIGH;
        else
            SDA_LOW;    
        
        HIGH_HOLD_DELAY;
        SCL_HIGH;
        HIGH_HOLD_DELAY;
            
        data=data<<1;
    }
     
    //ACK phase : 1 no ACK, 0 ACK
    SCL_LOW;
    SPACING_DELAY;
        
    SDA_HIGH;        
    HIGH_HOLD_DELAY;
    SDADDR &=~(1<<SDA)  ; // set to input
    SCL_HIGH;
    HIGH_HOLD_DELAY;    
    
    bool ack=!(SDAPIN & (1<<SDA));
    
    SCL_LOW;
    HIGH_HOLD_DELAY;
    SDADDR |=(1<<SDA)  ; // set to output
    return ack;
     
}
 
 
uint8_t I2C::ReadByte(uint8_t ack)
{
    SDADDR &=~(1<<SDA)  ; // set to input
    uint8_t data=0x00;
    uint8_t i;
            
    for(i=0;i<8;i++)
    {
        SCL_LOW;
        HIGH_HOLD_DELAY;
        SCL_HIGH;
        HIGH_HOLD_DELAY;
            
        if(SDAPIN &(1<<SDA))
            data|=(0x80>>i);
    }
        
    SCL_LOW;
    SPACING_DELAY;
    SDADDR |=(1<<SDA)  ; // set to output
    // generate an ACK if needed
    if(ack) SDA_LOW;    
    else    SDA_HIGH;

    HIGH_HOLD_DELAY;
    SCL_HIGH;
    HIGH_HOLD_DELAY;
    SCL_LOW;
    HIGH_HOLD_DELAY;
            
    return data;
}


void I2C::StartTransmission(uint8_t address, EI2C_ReadWrite rw){
    memset (buffer_, 0, sizeof(buffer_)); 
    buffer_pos_ = 0;
    start_acknowledged_ = false;
    Begin(); // low-level IO port setup
    Start(); // I2C Protocol start condition
    uint8_t shift = address<<1; 
    //trailing bit will indicate a write (0) or a read (1)
    if(direction_read == rw) shift |= 0x01;
    
    
    start_acknowledged_ = WriteByte(shift);
}

uint8_t I2C::RequestFrom(uint8_t address, uint8_t count){
    uint8_t readCount = 0;
    StartTransmission(address, direction_read);
    return readCount;
}

void I2C::Write(uint8_t data){
    if(start_acknowledged_)    {
        if(buffer_pos_ < MAX_BUFFER_SIZE){
            buffer_[buffer_pos_++] = data;    
        }
        
    }
}
    
uint8_t I2C::EndTransmission(){
    uint8_t ret = 0; // TODO: extend return code
    for(uint8_t i = 0; i < buffer_pos_; i++){
        WriteByte(buffer_[i]);
    }
    
    Stop();  // reset SDA and SDL to their normal low state
    return ret;
}












