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
// This #include statement was automatically added by the Particle IDE.
#include "itg3200.h"
ITG3200 * itg3200;

void setup() {
    auto itgAddress = 0x68; // jumper pin to GND
    itg3200 = new ITG3200(true, itgAddress);
}

void loop() {
    itg3200->getRotations();
    itg3200->getTemperature();
    itg3200->powerMode(TEitg3200_powerMode_Sleep);
    delay(350);
    itg3200->powerMode(TEitg3200_powerMode_Normal);
}