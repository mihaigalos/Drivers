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