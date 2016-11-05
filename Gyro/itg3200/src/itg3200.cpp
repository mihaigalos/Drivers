/*****************************************************************************
* 
* This file is part of Drivers-Gyro.
* 
* Drivers-Gyro is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* Drivers-Gyro is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with Drivers-Gyro.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************************/
#include "application.h"
#include "itg3200.h"
#include <stdint.h>
using namespace std;

ITG3200::ITG3200(bool debug, char itgAddress){
    itgAddress_ = itgAddress;
    debug_ = debug;
    pinMode(D2, OUTPUT);
    digitalWrite(D2,HIGH);
    
    pinMode(D3, OUTPUT);
    digitalWrite(D3,LOW);
    delay(startupTime);
    if(debug_) Serial.begin(9600);
    
    Wire.setSpeed(CLOCK_SPEED_400KHZ);
    Wire.begin();
    
    if(debug_){
        char id = itgRead(itgAddress_, WHO_AM_I);  
        Serial.print("ID: "); Serial.println(id, HEX);
        Serial.println("X\tY\tZ\t\tTemperature "); 
    }
    
    itgWrite(itgAddress_, DLPF_FS, (DLPF_FS_SEL_0|DLPF_FS_SEL_1|DLPF_CFG_0));// scale for the outputs to +/-2000 degrees per second
    itgWrite(itgAddress_, SMPLRT_DIV, 9);// Sample rate to 100 hz
}

vector<int> ITG3200::getRotations(){
    vector<int> result;
    int xRate, yRate, zRate;
    
    //Read the x,y and z output rates from the gyroscope.
    xRate = readX();
    yRate = readY();
    zRate = readZ();
    result.push_back(xRate); result.push_back(yRate); result.push_back(zRate);
    if(debug_){
        Serial.print(String(xRate) + String('\t') + String(yRate) + String('\t') + String(zRate)+String("\t\t")); //Print the output rates to the terminal, seperated by a TAB character.
    }
    return result;  
}

float ITG3200::getTemperature(){
    float data =0;
    int16_t rawData =0;
    rawData = itgRead(itgAddress_, TEMP_H)<<8;
    rawData |= itgRead(itgAddress_, TEMP_L);  
    // from the datasheet : 35 degrees Celsius = (1/280) * -13200 + b; b = 82.142857. 
    
    data = (float)rawData/280 + 82.142857; 
    if(debug_){
        Serial.println(String(data)+" degC.");  
    } 
    return data;
}

void ITG3200::powerMode(TEitg3200_powerMode mode){
    auto data = itgRead(itgAddress_, POWER_MANAGEMENT);
    if(TEitg3200_powerMode_Sleep == mode){
        itgWrite(itgAddress_, POWER_MANAGEMENT, data | SLEEP_BIT);
    } else if (TEitg3200_powerMode_Normal){
        itgWrite(itgAddress_, POWER_MANAGEMENT, data & ~SLEEP_BIT);
        delay(startupTime);
    }
}

/******************************** Private Methods ********************************/

void ITG3200::itgWrite(char address, char registerAddress, char data){
    Wire.beginTransmission(address);
    Wire.write(registerAddress);
    Wire.write(data);
    Wire.endTransmission();
}

unsigned char ITG3200::itgRead(char address, char registerAddress){
    unsigned char data=0;
    
    Wire.beginTransmission(address);
    Wire.write(registerAddress);
    Wire.endTransmission();
    Wire.requestFrom(address, 1);
    
    if(Wire.available())    data = Wire.read();
    
    return data;
}

int ITG3200::readX(void){
    int data=0;
    data = itgRead(itgAddress_, GYRO_XOUT_H)<<8;
    data |= itgRead(itgAddress_, GYRO_XOUT_L);  
    return data;
}

int ITG3200::readY(void){
    int data=0;
    data = itgRead(itgAddress_, GYRO_YOUT_H)<<8;
    data |= itgRead(itgAddress_, GYRO_YOUT_L);  
    return data;
}

int ITG3200::readZ(void){
    int data=0;
    data = itgRead(itgAddress_, GYRO_ZOUT_H)<<8;
    data |= itgRead(itgAddress_, GYRO_ZOUT_L);  
    return data;
}