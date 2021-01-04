#include <avr/io.h>
#include "simple_tea_v6.h"

uint8_t secret_data[] = {0xFE, 0x67, 0xAA, 0x45, 0x74, 0x32};
SimpleTEA_v6<sizeof(secret_data)> tea;

int main()
{
    secret_data[0] = PINB;
    auto crc = secret_data[0] | 0x55;
    tea.encrypt(secret_data, crc);
    if (secret_data[0] & (PINB))
    {
        PORTC = 0x05;
    }
    secret_data[1] = PINB & (1 << 2);
    tea.decrypt(secret_data, crc);
    if (secret_data[0] & (PINB))
    {
        PORTC = 0x50;
    }
}
