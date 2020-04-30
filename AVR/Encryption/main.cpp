// #include <iostream>
#include "simple_tea.h"

int main()
{
    uint8_t v[] = {0xFE, 0x67};
    unsigned int rounds{16};
    uint8_t key[] = {0x45, 0x74, 0x32, 0x11, 0x98, 0x94, 0xAB, 0xCF, 0x90, 0xAE, 0xBA, 0xDC, 0x06, 0x16, 0x81, 0x95};

    encrypt(rounds, v, key);

    // std::cout << "Encrypted: " << std::endl;
    // std::cout << std::hex << static_cast<uint16_t>(v[0]) << "    " << static_cast<uint16_t>(v[1]) << std::endl;

    decrypt(rounds, v, key);

    // std::cout << "Decrypted: " << std::endl;
    // std::cout << std::hex << static_cast<uint16_t>(v[0]) << "    " << static_cast<uint16_t>(v[1]) << std::endl;
}