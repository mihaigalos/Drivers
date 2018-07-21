#pragma once

static constexpr auto kEEPROMMetadataAddress = 0;

typedef struct {
  uint8_t timezone_sign : 1; // 1=+, 0=-
  uint8_t utc_offset : 4;    // +/- raw offset to UTC time, not accounting for
                             // daylight saving
  uint8_t is_daylight_saving_active : 1; // 0=no, 1=yes, if date is > last
                                         // sunday of March and < Last sunday of
                                         // October
  uint8_t is_china_time : 1; // neccessary, because china has no daylight
                             // saving, so depending on
  // is_daylight_saving_active, diff might be +8h or +7h.
  uint8_t unused : 1;
} TimeZoneInfo;

typedef union {
  TimeZoneInfo s_timezone_info;
  uint8_t u_timezone_info;
} UTimeZoneInfo;

typedef struct {
  uint8_t major : 3;
  uint8_t minor : 3;
  uint8_t patch : 2;
} VersionInfo;

typedef union {
  VersionInfo s_version_info;
  uint8_t u_version_info;
} UVersionInfo;

enum class DeviceType : uint8_t { Unknown, DotPhat, DotStix };

typedef struct SEEPROMMetadata {
  VersionInfo metadata_version_info;
  DeviceType device_type;

  VersionInfo software_version;
  TimeZoneInfo sofware_timezone_info;
  uint8_t software_version_last_updated_timestamp[4];

  VersionInfo hardware_version;
  TimeZoneInfo hardware_timezone_info;
  uint8_t hardware_version_timestamp[4];

  bool operator!=(const struct SEEPROMMetadata &rhs) const {
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&rhs);
    for (uint16_t i = 0; i < sizeof(*this); ++i) {
      if (*(p + i) != *(reinterpret_cast<const uint8_t *>(this) + i))
        return true;
    }
    return false;
  }

  bool is_valid_sw_timestamp() {
    return 0xFF != software_version_last_updated_timestamp[0] &&
           0x00 != software_version_last_updated_timestamp[0] &&
           0xFF != software_version_last_updated_timestamp[1] &&
           0x00 != software_version_last_updated_timestamp[1] &&
           0xFF != software_version_last_updated_timestamp[2] &&
           0x00 != software_version_last_updated_timestamp[2] &&
           0xFF != software_version_last_updated_timestamp[3] &&
           0x00 != software_version_last_updated_timestamp[3];
  }
#ifdef ARDUINO
  String to_hex() {
    String result;
    for (uint8_t i = 0; i < sizeof(*this); ++i) {
      int i_element = *(reinterpret_cast<uint8_t *>(this) + i);
      String s_element{i_element, HEX};

      if (i_element < 16)
        result += "0";

      result += s_element + " ";
    }
    return result;
  }
#endif
} EEPROMMetadata;