#pragma once

static constexpr auto kEEPROMMetadataAddress = 0;

typedef struct
{
  uint8_t timezone_sign : 1;             // 1=+, 0=-
  uint8_t utc_offset : 4;                // +/- raw offset to UTC time, not accounting for
                                         // daylight saving
  uint8_t is_daylight_saving_active : 1; // 0=no, 1=yes, if date is > last
                                         // sunday of March and < Last sunday of
                                         // October
  uint8_t is_china_time : 1;             // neccessary, because china has no daylight
                                         // saving, so depending on
  // is_daylight_saving_active, diff might be +8h or +7h.
  uint8_t unused : 1;
} TimeZoneInfo;

typedef union {
  TimeZoneInfo s_timezone_info;
  uint8_t u_timezone_info;
} UTimeZoneInfo;

using SoftwareLastUpdatedTimestamp = uint8_t[4];

typedef struct
{
  uint8_t major : 3;
  uint8_t minor : 3;
  uint8_t patch : 2;
} VersionInfo;

using HardwareVersion = VersionInfo;
using SoftwareVersion = VersionInfo;
using MetadataVersion = VersionInfo;

typedef union {
  VersionInfo s_version_info;
  uint8_t u_version_info;
} UVersionInfo;

enum class DeviceType : uint8_t
{
  Unknown,
  DotPhat,
  DotStix,
  DotShine
};

using UnitName = char[8];

enum class InstalledCapacity : uint8_t
{
  None,
  Fahrads_1,
  Invalid
};

using HarvestingCapability = uint8_t;

typedef struct
{
  InstalledCapacity installed_capacity : 3;
  HarvestingCapability harversting_capability_x_15mW : 5;
} EnergyInfo;

typedef union {
  EnergyInfo s_energy_info;
  uint8_t u_energy_info;
} UEnergyInfo;

using PositionDegrees = float;

typedef struct
{
  PositionDegrees latitude;
  PositionDegrees longitude;
} GPSPosition;

using PositionAdditionalInfo = char[16];

typedef struct
{
  uint8_t temperature_sensor : 1;
  uint8_t ultraviolet_sensor : 1;
  uint8_t eeprom : 1;
  uint8_t piezo_speaker : 1;

  uint8_t crypto_module : 1;
  uint8_t high_precision_time_reference : 1;
  uint8_t reset_pushbutton : 1;
  uint8_t act_pushbutton : 1;
} InstalledDevices;

typedef union {
  InstalledDevices s_installed_devices;
  uint8_t u_installed_devices;
} UInstalledDevices;

typedef struct
{
  uint8_t usb : 1;
  uint8_t external_antenna : 1;
  uint8_t antenna_calibration : 1;
  uint8_t solar_panel : 1;
} InstalledDevices2;

typedef struct
{
  uint8_t usb_power : 1;
  uint8_t outA : 1;
  uint8_t outB : 1;
  uint8_t reset : 1;

  uint8_t rgb : 1;
  uint8_t tx : 1;
  uint8_t rx : 1;
  uint8_t reserved : 1;
} InstalledLeds;

typedef struct SEEPROMMetadata
{
  MetadataVersion metadata_version_info;
  DeviceType device_type;

  SoftwareVersion software_version;
  TimeZoneInfo sofware_timezone_info;
  uint8_t software_version_last_updated_timestamp[4];

  HardwareVersion hardware_version;
  TimeZoneInfo hardware_timezone_info;
  uint8_t hardware_version_timestamp[4]; // hardware_version_timestamp : add 8 hours to the PCB manufactureing time
                                         // in China

  UnitName unit_name;
  UEnergyInfo energy_info;

  GPSPosition gps_position;
  PositionAdditionalInfo position_additional_info;
  InstalledDevices installed_devices;
  InstalledDevices2 installed_devices2;

  InstalledLeds installed_leds;

  bool operator!=(const struct SEEPROMMetadata &rhs) const
  {
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&rhs);
    for (uint16_t i = 0; i < sizeof(*this); ++i)
    {
      if (*(p + i) != *(reinterpret_cast<const uint8_t *>(this) + i))
        return true;
    }
    return false;
  }

  bool is_valid_sw_timestamp()
  {
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
  String to_hex()
  {
    String result;
    for (uint8_t i = 0; i < sizeof(*this); ++i)
    {
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

void update_eeprom_config(const EEPROMMetadata &current_configuration, const EEPROMMetadata &e2prom_metadata)
{
  EEPROM.get(kEEPROMMetadataAddress, e2prom_metadata);
  if (current_configuration != e2prom_metadata)
  {
    EEPROM.put(kEEPROMMetadataAddress, current_configuration);
  }
}