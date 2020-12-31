#include "SoftwareUart.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#pragma message "assuming clock: " STR(F_CPU) " Mhz."

#if defined(RX_PIN) || defined(TX_PIN)

static inline void uart_init_clk_div64__38400bps()
{
    CLKPR = (1 << CLKPCE) & 0x80;
    CLKPR = (1 << CLKPS2) | (1 << CLKPS1);
}

void uart_init()
{
#ifdef TX_PIN
    UART_DDR |= (1 << TX_PIN);
    UART_OUT_PORT_MAPPING |= (1 << TX_PIN); // Tx line high when idle
#endif

#ifdef RX_PIN
    UART_DDR &= ~(1 << RX_PIN);
#endif

    uart_init_clk_div64__38400bps();
}
#endif

void uart_write(uint8_t c)
{
    uint8_t bit_position{8};
    UART_OUT_PORT_MAPPING = 0 << TX_PIN; // low for start bit
#ifdef HIGH_ACCURACY
    asm("nop"); // balance time after start bit_
    asm("nop");
    asm("nop");
#endif
    do
    {
        uint8_t bit = c & 0x01;
        UART_OUT_PORT_MAPPING = bit << TX_PIN;
        c >>= 1;
    } while (--bit_position);
#ifdef HIGH_ACCURACY
    asm("nop");
#endif
    UART_OUT_PORT_MAPPING |= (1 << TX_PIN);
}