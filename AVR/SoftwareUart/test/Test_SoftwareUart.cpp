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
 * Test_SoftwareUart.cpp
 *
 * Created: 4/25/2017 7:10:58 PM
 *  Author: Mihai Galos
 */

#include "SoftwareUart.h"

void self_test_sw_uart_send() {
  uart_init();

  for (int i = 'a'; i <= 'z'; ++i) {
    uart_write(i);
  }

  for (volatile long long i = 0; i < 1000UL; ++i)
    ;
}

void self_test_sw_uart_receive_send() {
  uint8_t buf[7];
  for (uint8_t i = 0; i < sizeof(buf); ++i) {
    buf[i] = uart_read();
  }
  for (uint8_t i = 0; i < sizeof(buf); ++i) {
    uart_write(buf[i]);
  }
}