/**
 * Author: Mihai Galos, Joonas Pihlajamaa
 * Inspired by http://codeandlife.com
 * Inspired by V-USB example code by Christian Starkjohann
 * License: GNU GPL v3 (see License.txt)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// this is libusb, see http://libusb.sourceforge.net/
#include <usb.h>

#include <chrono>
#include <ctime>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <thread>

#include <cmath>
#include <functional>
#include <iterator>
#include <sstream>
#include <vector>
#include <memory>
#include <map>

#include <algorithm> // for std::remove
#include <chrono>    // std::chrono::seconds
#include <conio.h>   // for kbhit
#include <thread>    // std::this_thread::sleep_for
#include <time.h>    // for unix timestamps

#include "eeprom_metadata.h"
#include "i_usbRequest.h"
#include "command.h"
using namespace std;

constexpr uint16_t vendor_id {0x16C0};
constexpr uint16_t device_id {0x05DC};

// used to get descriptor strings for device identification
static int usbGetDescriptorString(usb_dev_handle *dev, int index, int langid,
                                  char *buf, int buflen) {
  char buffer[256];
  int rval, i;

  // make standard request GET_DESCRIPTOR, type string and given index
  // (e.g. dev->iProduct)
  rval = usb_control_msg(dev,
                         USB_TYPE_STANDARD | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
                         USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) + index,
                         langid, buffer, sizeof(buffer), 1000);

  if (rval < 0) // error
    return rval;

  // rval should be bytes read, but buffer[0] contains the actual response size
  if ((unsigned char)buffer[0] < rval)
    rval = (unsigned char)buffer[0]; // string is shorter than bytes read

  if (buffer[1] != USB_DT_STRING) // second byte is the data type
    return 0;                     // invalid return type

  // we're dealing with UTF-16LE here so actual chars is half of rval,
  // and index 0 doesn't count
  rval /= 2;

  // lossy conversion to ISO Latin1
  for (i = 1; i < rval && i < buflen; i++) {
    if (buffer[2 * i + 1] == 0)
      buf[i - 1] = buffer[2 * i];
    else
      buf[i - 1] = '?'; // outside of ISO Latin1 range
  }
  buf[i - 1] = 0;

  return i - 1;
}

void onExit(std::vector<usb_dev_handle *> &handles) {
  for (auto &handle : handles) {
    usb_close(handle);
  }
  handles.clear();
}

static vector<usb_dev_handle *> usbOpenDevice(int vendor, char *vendorName,
                                              int product, char *productName) {
  struct usb_bus *bus;
  struct usb_device *dev;
  char devVendor[256], devProduct[256];

  usb_dev_handle *handle = NULL;
  vector<usb_dev_handle *> result;

  usb_init();
  usb_find_busses();
  usb_find_devices();

  for (bus = usb_get_busses(); bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      if (dev->descriptor.idVendor != vendor ||
          dev->descriptor.idProduct != product)
        continue;

      // we need to open the device in order to query strings
      if (!(handle = usb_open(dev))) {
        cerr << "Warning: cannot open USB device: " << usb_strerror() << endl;
        continue;
      }

      // get vendor name
      if (usbGetDescriptorString(handle, dev->descriptor.iManufacturer, 0x0409,
                                 devVendor, sizeof(devVendor)) < 0) {
        cerr << "Warning: cannot query manufacturer for device: "
             << usb_strerror() << endl;
        usb_close(handle);
        continue;
      }

      // get product name
      if (usbGetDescriptorString(handle, dev->descriptor.iProduct, 0x0409,
                                 devProduct, sizeof(devVendor)) < 0) {
        cerr << "Warning: cannot query product for device: " << usb_strerror()
             << endl;
        usb_close(handle);
        continue;
      }

      cout << "Found vendor: " << devVendor << endl;
      cout << "Found product: " << devProduct << endl << endl;

      if (string(devProduct) == string(productName))
        // return handle;
        result.push_back(handle);
      else {
        usb_close(handle);
        cerr << "Cannot find usb device from : \nVendor: " << vendorName << endl
             << "Product: " << productName << endl;
      }
    }
  }

  return result;
}

void printReceivedBytes(uint16_t start_address, uint16_t nBytes, char buffer[],
                        string separator = "", bool print_bytecount = true) {
  if (print_bytecount)
    cout << "Got bytes: " << endl;
  const uint16_t kIntelHexByteCount = 0x10;
  const uint16_t kAddressIncrement = 0x10;

  uint16_t crc = kIntelHexByteCount;
  uint16_t start_offset =
      static_cast<uint16_t>(static_cast<double>(start_address) /
                            static_cast<double>(columnCount)) *
      columnCount;

  crc += start_offset >> 8;
  crc += static_cast<uint8_t>(start_offset);

  cout << uppercase << hex;

  auto printCrc = [&](uint16_t iteration_mod_column_count) {
    crc = -crc;
    crc = static_cast<uint8_t>(crc);

    if (crc < 16)
      cout << "0";

    cout << crc << endl;

    crc = kAddressIncrement;

    uint16_t foo = iteration_mod_column_count + start_offset;

    crc += foo >> 8;
    crc += static_cast<uint8_t>(foo);

  };

  auto printNumberOfBytes = [&](uint16_t i) {
    uint16_t div16 =
        static_cast<uint16_t>(static_cast<double>(i + start_address) /
                              static_cast<double>(kAddressIncrement)) *
        kAddressIncrement;

    cout << ":10";

    if (!(0xF000 & div16)) {
      cout << "0";
      if (!(0x0F00 & div16)) {
        cout << "0";
        if (!(0x00F0 & div16)) {
          cout << "0";
        }
      }
    }

    cout << div16;
  };

  auto printRecordType = [&] { cout << "00"; };

  for (uint16_t i = 0; i < nBytes; ++i) {

    if (0 == i % 16) {

      if (i > 0)
        printCrc(i);
      printNumberOfBytes(i);
      printRecordType();
    }

    auto value = static_cast<uint16_t>(static_cast<uint8_t>(buffer[i]));
    crc += value;

    if (value < 16)
      cout << "0";
    cout << value;
    cout << separator;
  }

  printCrc(1);
  cout << dec << nouppercase;
}

vector<string> tokenize_string(const string &s) {
  std::stringstream ss(s);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> vstrings(begin, end);

  return vstrings;
}

void run_until_keypressed(std::function<void()> callable) {
  do {
    callable();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  } while (!kbhit());
}

void looping_dump(usb_dev_handle *handle, char *buffer){
  cout << "Looping dump. Press ESC to exit." << endl << endl;
  auto callable = [&] {
    usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
                                         USB_ENDPOINT_IN,
                             static_cast<int>(USBRequest::DATA_OUT), 0, 0,
                             (char *)buffer, sizeof(buffer), 5000);
    static string old_buffer;
    if (string{buffer} != old_buffer)
    {
      cout << "Got bytes: " << buffer << endl;
      old_buffer = string{buffer};
    }

  };

  run_until_keypressed(callable);
  std::this_thread::sleep_for(std::chrono::seconds(1));
}


auto getUsbHandles() -> std::vector<usb_dev_handle *>{
  auto device_handles = usbOpenDevice(
        vendor_id, const_cast<char *>(string{"Galos Industries"}.c_str()),
        device_id, const_cast<char *>(string{"DotPhat"}.c_str()));

    if (0 == device_handles.size()) {
      throw std::runtime_error("Could not find USB device!");
    } else {

      cout << "Found \033[1;36m" << device_handles.size() << "\033[0m device";

      if (1 < device_handles.size()) {
        cout << "s";
      }

      cout << "." << endl;
      for (uint8_t i = 0; i < device_handles.size(); ++i) {
        cout << to_string(static_cast<long long>(i)) << ": " << hex
             << device_handles.at(i) << dec << endl;
      }
    }

    return device_handles;
}



int main(int argc, char **argv) {


  int nBytes = 0;

  cout << "Usage:" << endl;
  cout << "  exit" << endl;
  cout << "  flashdump <direct hex address literals>" << endl;
  cout << "  in <predicate - see below>: send to usb device" << endl;
  cout << "     s:[frequence in millisecond multiples of 100ms[:max count]]:<string>: send over radio" << endl;
  cout << "       example: in s:5:HelloWorld"<<endl;
  cout << "     r: receive over radio" << endl;
  cout << "  list" << endl;
  cout << "  off" << endl;
  cout << "  on" << endl;
  cout << "  out: looping read from usb device" << endl;
  cout << "  oute: read and parse eeprom metadata" << endl;
  cout << "  use <device index>" << endl;
  cout << "  reset" << endl << endl;

  string command = "list";
  string parameters;

  Command::init();

  do {

    auto start = chrono::high_resolution_clock::now();
    auto command_with_parameters = tokenize_string(command);

    command_map[command_with_parameters[0]]()->execute(command_with_parameters);
    // if ("on" == command_with_parameters[0]) {
    //   nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
    //                                        USB_ENDPOINT_IN,
    //                            static_cast<int>(USBRequest::LED_ON), 0, 0,
    //                            (char *)buffer, sizeof(buffer), 5000);
    // } else if ("off" == command_with_parameters[0]) {
    //   nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
    //                                        USB_ENDPOINT_IN,
    //                            static_cast<int>(USBRequest::LED_OFF), 0, 0,
    //                            (char *)buffer, sizeof(buffer), 5000);
    // } else if ("out" == command_with_parameters[0]) {
    //   looping_dump(handle, buffer);
    // } else if ("oute" == command_with_parameters[0]) {
    //
    //   nBytes = usb_control_msg(
    //         handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
    //         static_cast<int>(USBRequest::DATA_WRITE), 0, 0,
    //         const_cast<char *>("e"),
    //         2, 5000);
    //
    //   cout << "Read and parsing eeprom metadata:" << endl << endl;
    //   nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
    //                                        USB_ENDPOINT_IN,
    //                            static_cast<int>(USBRequest::DATA_OUT), 0, 0,
    //                            (char *)buffer, sizeof(buffer), 5000);
    //
    //   cout << "Got bytes: " << buffer << endl;
    //   auto eeprom_metadata = tokenize_string(buffer);
    //
    //   auto print_version = [](string tokenized_element, string text) {
    //     uint16_t one_byte = 0;
    //     std::istringstream iss(tokenized_element);
    //     iss >> std::hex >> one_byte;
    //
    //     UVersionInfo version_info;
    //     version_info.u_version_info = one_byte;
    //
    //     auto metadata_version = version_info.s_version_info;
    //
    //     cout << text << " ";
    //     cout << "\033[1;33m"
    //          << "\033[1;46m"
    //          << "v" << static_cast<uint16_t>(metadata_version.major) << "."
    //          << static_cast<uint16_t>(metadata_version.minor) << "."
    //          << static_cast<uint16_t>(metadata_version.patch) << "\033[0m"
    //          << " [Raw: 0x" << hex << one_byte << "]" << dec << endl;
    //
    //   };
    //
    //   uint8_t one_byte = 0;
    //   std::istringstream iss(eeprom_metadata.at(0));
    //   iss >> std::hex >> one_byte;
    //
    //   print_version(eeprom_metadata.at(0), "Metadata version:");
    //   print_version(eeprom_metadata.at(2), "Sofware version: ");
    //   print_version(eeprom_metadata.at(8), "Hardware version:");
    //
    //   auto print_timestamp = [](vector<string> &tokenized_elements,
    //                             string time_zone_info, string text) {
    //
    //     cout << endl << text;
    //     time_t epoch = 0;
    //     uint8_t i = 0;
    //     for (auto &element : tokenized_elements) {
    //       uint16_t one_byte = 0;
    //       std::istringstream iss(element);
    //       iss >> std::hex >> one_byte;
    //
    //       epoch |= one_byte << (3 - i++) * 8;
    //     }
    //
    //     uint16_t uint16_tzi = 0;
    //     std::istringstream iss(time_zone_info);
    //     iss >> std::hex >> uint16_tzi;
    //
    //     UTimeZoneInfo tzi;
    //     tzi.u_timezone_info = uint16_tzi;
    //
    //     string utc_sign = tzi.s_timezone_info.timezone_sign ? "+" : "-";
    //     auto time_offset = tzi.s_timezone_info.utc_offset;
    //
    //     if (!tzi.s_timezone_info.timezone_sign)
    //       time_offset = -time_offset;
    //
    //     string is_daylight_saving;
    //     if (tzi.s_timezone_info.is_daylight_saving_active &&
    //         !tzi.s_timezone_info.is_china_time) {
    //       time_offset += 1;
    //       is_daylight_saving = " (+1 Daylight saving included)";
    //     } else {
    //       is_daylight_saving = " No Daylight saving";
    //     }
    //
    //     std::tm myEpoch = *std::gmtime(&epoch);
    //     if (tzi.s_timezone_info.is_daylight_saving_active) {
    //       myEpoch.tm_hour = myEpoch.tm_hour - tzi.s_timezone_info.utc_offset;
    //       if (tzi.s_timezone_info.is_china_time) {
    //         --myEpoch.tm_hour;
    //       }
    //     }
    //
    //     auto make_time = mktime(&myEpoch);
    //     string c_time = std::ctime(&make_time);
    //
    //     c_time.erase(std::remove(c_time.begin(), c_time.end(), '\n'),
    //                  c_time.end());
    //
    //     cout << "\033[1;36m" << c_time
    //
    //          << "\033[1;35m"
    //          << " UTC" << utc_sign
    //          << to_string(static_cast<uint32_t>(time_offset)) << "\033[1;36m"
    //          << is_daylight_saving << "."
    //          << "\033[0m" << endl
    //          << "[Raw hex: " << hex << static_cast<uint32_t>(epoch) << dec
    //          << "]";
    //
    //     cout << endl;
    //   };
    //
    //   vector<string> software_version_last_updated_timestamp{
    //       eeprom_metadata.begin() + 4, eeprom_metadata.begin() + 8};
    //   vector<string> hardware_version_timestamp{eeprom_metadata.begin() + 10,
    //                                             eeprom_metadata.begin() + 14};
    //
    //   auto sofware_timezone_info = eeprom_metadata.at(3);
    //   auto hardware_timezone_info = eeprom_metadata.at(9);
    //
    //   print_timestamp(software_version_last_updated_timestamp,
    //                   sofware_timezone_info, "SW Timestamp : ");
    //   print_timestamp(hardware_version_timestamp, hardware_timezone_info,
    //                   "HW Timestamp : ");
    //
    // } else if ("in" == command_with_parameters[0]) {
    //   if (2 == command_with_parameters.size()) {
    //     nBytes = usb_control_msg(
    //         handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
    //         static_cast<int>(USBRequest::DATA_WRITE), 0, 0,
    //         const_cast<char *>(command_with_parameters.at(1).c_str()),
    //         command_with_parameters.at(1).length() + 1, 5000);
    //   }
    // } else if ("clear" == command_with_parameters[0]) {
    //   std::cout << "\x1B[2J\x1B[H";
    // } else if ("flashdump" == command_with_parameters[0]) {
    //
    //   if (argc > 2) {
    //     nBytes = usb_control_msg(
    //         handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
    //         static_cast<int>(USBRequest::FLASH_DUMP_FROM_ADDRESS), 0, 0,
    //         const_cast<char *>(command_with_parameters.at(1).c_str()),
    //         command_with_parameters.at(1).length() + 1, 5000);
    //   } else {
    //     char address_hex[6] = "0";
    //     constexpr double atmega328p_flash_size = 32 * 1024;
    //     constexpr uint16_t repeat_count = static_cast<uint16_t>(
    //         ceil(atmega328p_flash_size / static_cast<double>(kBufferSize)));
    //
    //     for (uint16_t i = 0; i < repeat_count; ++i) {
    //       uint16_t offset = i * kBufferSize;
    //       sprintf(address_hex, "%x", offset);
    //
    //       nBytes = usb_control_msg(
    //           handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
    //           static_cast<int>(USBRequest::FLASH_DUMP_FROM_ADDRESS), 0, 0,
    //           address_hex, strlen(address_hex) + 1, 5000);
    //
    //       nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
    //                                            USB_ENDPOINT_IN,
    //                                static_cast<int>(USBRequest::DATA_OUT), 0, 0,
    //                                (char *)buffer, sizeof(buffer), 5000);
    //       printReceivedBytes(offset, nBytes, buffer, "", false);
    //     }
    //   }
    //
    // } else if ("reset" == command_with_parameters[0]) {
    //   nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
    //                                        USB_ENDPOINT_IN,
    //                            static_cast<int>(USBRequest::RESET), 0, 0,
    //                            (char *)buffer, sizeof(buffer), 5000);
    // } else if ("list" == command_with_parameters[0]) {
    //   onExit(device_handles);
    //
    //   device_handles = usbOpenDevice(
    //       vendor_id, const_cast<char *>(string{"Galos Industries"}.c_str()),
    //       device_id, const_cast<char *>(string{"DotPhat"}.c_str()));
    //
    //   if (0 == device_handles.size()) {
    //     throw std::runtime_error("Could not find USB device!");
    //   } else {
    //
    //     cout << "Found \033[1;36m" << device_handles.size() << "\033[0m device";
    //
    //     if (1 < device_handles.size()) {
    //       cout << "s";
    //     }
    //
    //     cout << "." << endl;
    //     for (uint8_t i = 0; i < device_handles.size(); ++i) {
    //       cout << to_string(static_cast<long long>(i)) << ": " << hex
    //            << device_handles.at(i) << dec << endl;
    //     }
    //
    //     if (nullptr == handle && device_handles.size() > 0) {
    //       handle = device_handles.at(0);
    //     }
    //   }
    //
    // } else if ("use" == command_with_parameters[0]) {
    //   desired_device_index = stoi(command_with_parameters[1]);
    //   if (desired_device_index < device_handles.size()) {
    //     handle = device_handles[desired_device_index];
    //   } else {
    //     cout << "Invalid index!" << endl;
    //   }
    // }

    if (nBytes < 0)
      fprintf(stderr, "USB error: %s\n", usb_strerror());

    auto elapsed = chrono::high_resolution_clock::now() - start;
    long long milliseconds =
        chrono::duration_cast<chrono::milliseconds>(elapsed).count();
    cout << endl << "CPU time used: " << milliseconds << " ms\n";

    cout << "> ";

    getline(std::cin, command);

  } while ("exit" != command);

  return 0;
}
