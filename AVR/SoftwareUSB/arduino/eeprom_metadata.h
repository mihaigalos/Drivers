#pragma once

static constexpr auto kEEPROMMetadataAddress = 0;

typedef struct{
  uint8_t major: 3;
  uint8_t minor: 3;
  uint8_t patch: 2;
}VersionInfo;

typedef union{
  VersionInfo s_version_info;
  uint8_t u_version_info;
}UVersionInfo;

enum class DeviceType : uint8_t { Unknown, DotPhat, DotStix};

typedef struct SEEPROMMetadata {
  VersionInfo metadata_version_info;
  DeviceType device_type;
  VersionInfo software_version;
  uint8_t software_version_last_updated_timestamp[4];
  VersionInfo hardware_version;
  uint8_t hardware_version_timestamp[4];

  bool operator!=(const struct SEEPROMMetadata & rhs) const{
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&rhs);
    for( uint16_t i =0 ; i< sizeof(*this) ; ++i ){
      if(*(p + i) != *(reinterpret_cast<const uint8_t*>(this) + i)) return true;
    }
    return false;
  }

  bool is_valid_sw_timestamp(){
    return  0xFF != software_version_last_updated_timestamp[0] && 0x00 != software_version_last_updated_timestamp[0] &&
            0xFF != software_version_last_updated_timestamp[1] && 0x00 != software_version_last_updated_timestamp[1] &&
            0xFF != software_version_last_updated_timestamp[2] && 0x00 != software_version_last_updated_timestamp[2] &&
            0xFF != software_version_last_updated_timestamp[3] && 0x00 != software_version_last_updated_timestamp[3];
  }
#ifdef ARDUINO
  String to_hex()
  {
      String result;
      for (uint8_t i = 0; i < sizeof(*this); ++i){
        int i_element = *(reinterpret_cast<uint8_t*>(this) + i);
        String s_element {i_element, HEX};

        if(i_element < 16)
            result += "0";

        result += s_element+ " ";
     }
     return result;
  }
#endif
}EEPROMMetadata;