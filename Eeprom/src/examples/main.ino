/*****************************************************************************
* 
* This file is part of E2PROM.
* 
* E2PROM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* E2PROM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with E2PROM.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************************/

#include "e2prom.h"

E2PROM *e = NULL;

#define OWNADDRESS 0x50 // 127 maximum devices, 0b0101000 << 1 + 1 bit R/W (automatically added by Wire) = 0x50

void setup() {
    e = new E2PROM(OWNADDRESS); 
    delay(1000);
    
    uint8_t buffer[] = {"Hello World!"};
    e->writePage(0, &buffer[0], 8);
}

void loop() {
    e->dump();
    
    delay(2000);
}