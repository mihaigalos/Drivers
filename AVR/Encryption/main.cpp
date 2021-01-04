#include <avr/io.h>
#include "simple_tea_v7.h"

uint8_t publicKey[kPublicKeySize]{0xa6, 0x6f, 0x23, 0x9f, 0x8b, 0x7f, 0xb1, 0x50};
uint8_t secret_data[] = {0xFE, 0x67, 0xAA, 0x45, 0x74, 0x32};
SimpleTEA_v7<sizeof(secret_data)> tea;

int main()
{
    secret_data[0] = PINB;
    auto crc = secret_data[0] | 0x55;
    tea.encrypt(publicKey, secret_data, crc);
    if (secret_data[0] & (PINB))
    {
        PORTB = 0x05;
    }
    secret_data[1] = PINB & (1 << 2);
    tea.decrypt(publicKey, secret_data, crc);
    if (secret_data[0] & (PINB))
    {
        PORTB = 0x50;
    }
}
