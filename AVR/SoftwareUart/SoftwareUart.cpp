/*****************************************************************************
*
* This file is part of SoftwareUart.
*
* SoftwareUart is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* SoftwareUart is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SoftwareUart.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************************/

/*
 * SoftwareUart.cpp
 *
 * Created: 4/20/2017 9:29:03 PM
 *  Author: Mihai Galos
 */

#include "SoftwareUart.h"

#ifndef F_CPU
#error Please #define F_CPU
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#pragma message "assuming clock: " STR(F_CPU) " Hz, baud: " STR(FIXED_BAUD_RATE)

// All function implementations are in SoftwareUart.S