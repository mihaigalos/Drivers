#include "SoftwareUart.h"

int main()
{
    uart_init();
    do
    {
        for (uint8_t c = 'a'; c < 'z'; ++c)
        {
            uart_write(c);
        }
        for (volatile uint16_t i = 0; i < 15000; ++i)
            ;
    } while (1);
}
