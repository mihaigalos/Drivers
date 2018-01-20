/*****************************************************************************
*
* This file is part of Drivers-Gyro-ITG3200.
*
* Drivers-Gyro-ITG3200 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Drivers-Gyro-ITG3200 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Drivers-Gyro-ITG3200.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************************/
#include <vector>

#define DEFAULT_ITG3200_ADDRESS 0x69 // jumper to VDD by default
typedef enum {
  TEitg3200_powerMode_Unknown,
  TEitg3200_powerMode_Normal,
  TEitg3200_powerMode_Sleep
} TEitg3200_powerMode;
class ITG3200 {
  // Register addresses
  static char const WHO_AM_I = 0x00;
  static char const SMPLRT_DIV = 0x15;
  static char const DLPF_FS = 0x16;
  static char const GYRO_XOUT_H = 0x1D;
  static char const GYRO_XOUT_L = 0x1E;
  static char const GYRO_YOUT_H = 0x1F;
  static char const GYRO_YOUT_L = 0x20;
  static char const GYRO_ZOUT_H = 0x21;
  static char const GYRO_ZOUT_L = 0x22;

  static char const TEMP_H = 0x1B;
  static char const TEMP_L = 0x1C;
  static char const POWER_MANAGEMENT = 0x3E;

  // Configuration

  // Set DLPF_CFG to 3 for 1kHz Fint and 42 Hz Low Pass Filter
  static char const DLPF_CFG_0 = (1 << 0); // DLPF, Full Scale Register Bits
  static char const DLPF_CFG_1 = (1 << 1);
  static char const DLPF_CFG_2 = (1 << 2);

  static char const DLPF_FS_SEL_0 =
      (1 << 3); // FS_SEL must be set to 3 for correct operation
  static char const DLPF_FS_SEL_1 = (1 << 4);

  static char const SLEEP_BIT = (1 << 6);

  static char const startupTime = 50; // ms

  char itgAddress_;
  bool debug_;
  float selfHeatingAmount_;

  void itgWrite(char address, char registerAddress, char data);
  unsigned char itgRead(char address, char registerAddress);
  int readX();
  int readY();
  int readZ();

public:
  ITG3200(bool debug = false, char itgAddress = DEFAULT_ITG3200_ADDRESS,
          float selfHeatingAmount = 0);

  std::vector<int> getRotations();
  float getTemperature();
  void powerMode(TEitg3200_powerMode mode);
};