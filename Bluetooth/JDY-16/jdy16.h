#pragma once

#include "Arduino.h"

using function_pointer_write_i2c_n_bytes = uint8_t (*)(
    const uint8_t destination_address, const uint16_t register_address,
    const char data[], const uint8_t count);

using function_pointer_read_i2c_bytes =
    uint8_t (*)(const uint8_t destination_address,
                const uint16_t register_address, uint8_t bytes_to_read);
using function_pointer_read_data_length =
    uint8_t (*)(const uint8_t destination_address);

class JDY16 {
public:
  JDY16(const uint8_t &pin, const uint8_t &clockPrescaler,
        const function_pointer_write_i2c_n_bytes &f_write,
        const function_pointer_read_i2c_bytes &f_read,
        const function_pointer_read_data_length &f_length)
      : pin_{pin}, clockPrescaler_{clockPrescaler},
        pointer_write_i2c_n_bytes_{f_write}, pointer_read_i2c_bytes_{f_read},
        pointer_read_data_length_{f_length} {}

  void deepSleepNoBroadcast();
  void powerOnRequest(bool on);
  void readFromApp();
  void wakeUp();
  void writeToApp();

private:
  uint8_t readDataLength(const uint8_t destination_address);

  function_pointer_write_i2c_n_bytes pointer_write_i2c_n_bytes_{nullptr};
  function_pointer_read_i2c_bytes pointer_read_i2c_bytes_{nullptr};
  function_pointer_read_data_length pointer_read_data_length_{nullptr};
  const uint8_t &clockPrescaler_;
  const uint8_t &pin_;
};
