/*****************************************************************************
* 
* This file is part of Drivers-Gyro-ITG3200.
* 
* Drivers-Gyro-ITG3200 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* Drivers-Gyro-ITG3200 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with Drivers-Gyro-ITG3200.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************************/
// This #include statement was automatically added by the Particle IDE.
#include "itg3200.h"
ITG3200 * itg3200;

void setup() {
    auto debug = true;
    auto itgAddress = 0x68; // jumper pin to GND
    auto sensorSelfHeatingAmount = 3.7; // degrees Celsius, compensate for self-heating when computing temperature
    
    itg3200 = new ITG3200(debug, itgAddress, sensorSelfHeatingAmount);
}

void loop() {
    auto tic1 = micros();
    itg3200->getRotations();
    itg3200->getTemperature();
    itg3200->powerMode(TEitg3200_powerMode_Sleep);
    auto toc1 = micros();
    Serial.println("delta: "+String(toc1-tic1));
    delay(350);
    itg3200->powerMode(TEitg3200_powerMode_Normal);
}