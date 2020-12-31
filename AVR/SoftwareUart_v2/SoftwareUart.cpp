#include "SoftwareUart.h"

void uart_write(uint8_t c)
{
    uint8_t bit_position{8};
    UART_OUT_PORT_MAPPING = 0 << TX_PIN; // low for start bit
    do
    {
        uint8_t bit = c & 0x01;
        UART_OUT_PORT_MAPPING = bit << TX_PIN;
        c >>= 1;
    } while (--bit_position);
    UART_OUT_PORT_MAPPING |= (1 << TX_PIN);
}