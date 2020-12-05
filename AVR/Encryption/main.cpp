#include <avr/io.h>
#include "simple_tea_v4.h"

uint8_t secret_data[] = {0xFE, 0x67, 0xAA, 0x45, 0x74, 0x32, 0x11, 0x98, 0x94, 0x06, 0x16, 0x81, 0x95, 0x95};
SimpleTEA_v4<sizeof(secret_data) / 2> tea;

int main()
{

    unsigned int rounds{16};
    uint8_t key[] = {0x45, 0x74, 0x32, 0x11, 0x98, 0x94, 0xAB, 0xCF, 0x90, 0xAE, 0xBA, 0xDC, 0x06, 0x16, 0x81, 0x95};

    secret_data[0] = PINB;
    tea.encrypt(rounds, key, secret_data);
    if (secret_data[0] & (PINB))
    {
        PORTC = 0x05;
    }
    secret_data[1] = PINB & (1 << 2);
    tea.decrypt(rounds, key, secret_data);
    if (secret_data[0] & (PINB))
    {
        PORTC = 0x50;
    }
}
