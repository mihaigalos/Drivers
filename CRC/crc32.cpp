#include "crc32.h"

#include <iostream>
#include <fstream>
#include <vector>

int main(){
  
  //static inline uint32_t crc32 (uint32_t crc, uint8_t *serialized, int length=2) 
  //seek to the end
  std::ifstream file("blink.bin", std::ios::binary);
    file.seekg(0, std::ios::end);

    //Get the file size
    int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    //Reduce the file size by any header bytes that might be present
    fileSize -= file.tellg();

    std::vector<unsigned char> serialized;
    serialized.resize(fileSize);
    file.read((char *)&(serialized[0]), fileSize);
    file.close();
    
    uint32_t crc = 0, bytes = 0; uint16_t word = 0;
    for(const auto & i : serialized){
      word<<=8;
      word|=i;
      
      if(bytes> 0 && 0 == (bytes-1)%2){
        std::cout<<std::dec<<bytes<<":  "<<std::hex<<word<<std::endl;
        crc = crc32(crc, reinterpret_cast<uint8_t*>(&word));
      }
      
      ++bytes;
    }
    
    if(bytes> 0 && 0 == (bytes-1)%2){
      crc = crc32(crc, reinterpret_cast<uint8_t*>(&word));
    }

    std::cout<<std::hex<<"crc32 [representation = 0x"<<polynomial_representation<<"] : 0x"<<crc<<std::endl;
    
  return 0;
}