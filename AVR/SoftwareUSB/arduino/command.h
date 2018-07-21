#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <exception>

static constexpr uint8_t kCommandBufferSize=254u;

void onExit(std::vector<usb_dev_handle *> &handles);
auto getUsbHandles() -> std::vector<usb_dev_handle *>;
void printReceivedBytes(uint16_t start_address, uint16_t nBytes, char buffer[],
                        std::string separator, bool print_bytecount);
std::vector<std::string> tokenize_string(const std::string &s) {
  std::stringstream ss(s);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> vstrings(begin, end);

  return vstrings;
}

using EndpointIO = int;
struct TRunParameters{
  EndpointIO endpoint;
  USBRequest request;
  std::function<void()> postAction;
  uint8_t byte_count_override;
};

class Command {
public:

  static void init(){
    onExit(device_handles_);
    Command::device_handles_ = getUsbHandles();
    Command::handle_ = device_handles_.at(0);
  }

  auto execute(std::vector<std::string>& args = empty_vector_) -> int{
    auto result = 0;
    TRunParameters parameters = run (args);

    if(USBRequest::Unknown != parameters.request){
      uint8_t request_length = sizeof(buffer_);
      if(USBRequest::FLASH_DUMP_FROM_ADDRESS == parameters.request) request_length = parameters.byte_count_override;

      result = usb_control_msg(handle_, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
        static_cast<int>(parameters.endpoint),
        static_cast<int>(parameters.request),
        0, 0,
        (char *)buffer_, request_length, 5000);
    }

    if(parameters.postAction) parameters.postAction();
    return result;
  }
protected:
   virtual TRunParameters run(std::vector<std::string>& args) = 0;
   static usb_dev_handle * handle_;
   static std::vector<usb_dev_handle *> device_handles_;
   static char buffer_[kCommandBufferSize];
private:
   static std::vector<std::string> empty_vector_;

};

std::vector<std::string> Command::empty_vector_;
usb_dev_handle * Command::handle_ {nullptr};
char Command::buffer_[kCommandBufferSize];
std::vector<usb_dev_handle *> Command::device_handles_;


class ExitCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{EndpointIO(), USBRequest()};
  }
};


class OutCommandSimple : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{USB_ENDPOINT_IN, USBRequest::DATA_OUT};
  }
};

class OutCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{USB_ENDPOINT_IN, USBRequest::DATA_OUT, Utils::looping_dump};
  }
private:

  class Utils{
  public:
    static void run_until_keypressed(std::function<void()> callable) {
      do {
        callable();
        std::this_thread::sleep_for(std::chrono::seconds(1));
      } while (!kbhit());
    }

    static void looping_dump(){
      std::cout << "Looping dump. Press ESC to exit." << std::endl << std::endl;
      auto callable = [&] {
          OutCommandSimple{}.execute();
          static std::string old_buffer;

          if (std::string{buffer_} != old_buffer)
          {
            std::cout << "Got bytes: " << buffer_ << std::endl;
            old_buffer = std::string{buffer_};
          }

        };

        run_until_keypressed(callable);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  };


};

class FlashDumpCommandSimple : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    std::string arg = args.at(1);
    uint8_t length = kCommandBufferSize>arg.length()?arg.length():kCommandBufferSize;
    memcpy(buffer_,arg.c_str(), arg.length()+1);
    return TRunParameters{USB_ENDPOINT_OUT, USBRequest::FLASH_DUMP_FROM_ADDRESS,[](){},length};
  }
};

class FlashDumpCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {

     char address_hex[6];
     memset (address_hex, 0, sizeof(address_hex));
     address_hex[0] = '0';
         constexpr double atmega328p_flash_size = 32 * 1024;
         constexpr uint16_t repeat_count = static_cast<uint16_t>(
             ceil(atmega328p_flash_size / static_cast<double>(kBufferSize)));

         for (uint16_t i = 0; i < repeat_count; ++i) {
            uint16_t offset = i * kBufferSize;
            sprintf(address_hex, "%x", offset);
            std::vector<std::string> args;
            args.push_back(std::string{"irrelevant"});
            args.push_back(std::string{address_hex});
            FlashDumpCommandSimple{}.execute(args);
            OutCommandSimple{}.execute();

            printReceivedBytes(offset, kBufferSize, buffer_, "", false);
         }
    return TRunParameters{EndpointIO(), USBRequest()};
  }
};
class InCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    memcpy(buffer_,args.at(1).c_str(), kCommandBufferSize>args.at(1).length()?args.at(1).length():kCommandBufferSize);
    return TRunParameters{USB_ENDPOINT_OUT, USBRequest::DATA_WRITE};
  }
};


class OffCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{USB_ENDPOINT_IN, USBRequest::LED_OFF};
  }
};
class OnCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{USB_ENDPOINT_IN, USBRequest::LED_ON};
  }
};
class OuteCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    buffer_[0] = 'e';
    buffer_[1] = '\0';
    return TRunParameters{USB_ENDPOINT_OUT, USBRequest::DATA_WRITE, [&](){
      OutCommandSimple{}.execute();
      EepromParser::parse();
    }};
  }
private:
  class EepromParser{
  public:
    static void parse(){
        std::cout << "Got eeprom bytes: " << buffer_ << std::endl;
        std::vector<std::string> eeprom_metadata = tokenize_string(buffer_);

        print_versions(eeprom_metadata);
        print_timestamps(eeprom_metadata);
    }

  private:

  static void print_versions(const std::vector<std::string>& eeprom_metadata){
    uint8_t one_byte = 0;
    std::istringstream iss(eeprom_metadata.at(0));
    iss >> std::hex >> one_byte;

    print_version(eeprom_metadata.at(0), "Metadata version:");
    print_version(eeprom_metadata.at(2), "Sofware version: ");
    print_version(eeprom_metadata.at(8), "Hardware version:");
  }

  static void print_timestamps(const std::vector<std::string>& eeprom_metadata){
    std::vector<std::string> software_version_last_updated_timestamp{
        eeprom_metadata.begin() + 4, eeprom_metadata.begin() + 8};
    std::vector<std::string> hardware_version_timestamp{eeprom_metadata.begin() + 10,
                                              eeprom_metadata.begin() + 14};

    auto sofware_timezone_info = eeprom_metadata.at(3);
    auto hardware_timezone_info = eeprom_metadata.at(9);

    print_timestamp(software_version_last_updated_timestamp,
                    sofware_timezone_info, "SW Timestamp : ");
    print_timestamp(hardware_version_timestamp, hardware_timezone_info,
                    "HW Timestamp : ");
  }

  static void print_version (const std::string tokenized_element, const std::string text) {
    uint16_t one_byte = 0;
    std::istringstream iss(tokenized_element);
    iss >> std::hex >> one_byte;

    UVersionInfo version_info;
    version_info.u_version_info = one_byte;

    auto metadata_version = version_info.s_version_info;

    std::cout << text << " ";
    std::cout << "\033[1;33m"
        << "\033[1;46m"
        << "v" << static_cast<uint16_t>(metadata_version.major) << "."
        << static_cast<uint16_t>(metadata_version.minor) << "."
        << static_cast<uint16_t>(metadata_version.patch) << "\033[0m"
        << " [Raw: 0x" << std::hex << one_byte << "]" << std::dec << std::endl;

    }

    static void print_timestamp(std::vector<std::string> &tokenized_elements,
                              std::string time_zone_info, std::string text) {

      std::cout << std::endl << text;
      time_t epoch = 0;
      uint8_t i = 0;
      for (auto &element : tokenized_elements) {
        uint16_t one_byte = 0;
        std::istringstream iss(element);
        iss >> std::hex >> one_byte;

        epoch |= one_byte << (3 - i++) * 8;
      }

      uint16_t uint16_tzi = 0;
      std::istringstream iss(time_zone_info);
      iss >> std::hex >> uint16_tzi;

      UTimeZoneInfo tzi;
      tzi.u_timezone_info = uint16_tzi;

      std::string utc_sign = tzi.s_timezone_info.timezone_sign ? "+" : "-";
      auto time_offset = tzi.s_timezone_info.utc_offset;

      if (!tzi.s_timezone_info.timezone_sign)
        time_offset = -time_offset;

      std::string is_daylight_saving;
      if (tzi.s_timezone_info.is_daylight_saving_active &&
          !tzi.s_timezone_info.is_china_time) {
        time_offset += 1;
        is_daylight_saving = " (+1 Daylight saving included)";
      } else {
        is_daylight_saving = " No Daylight saving";
      }

      std::tm myEpoch = *std::gmtime(&epoch);
      if (tzi.s_timezone_info.is_daylight_saving_active) {
        myEpoch.tm_hour = myEpoch.tm_hour - tzi.s_timezone_info.utc_offset;
        if (tzi.s_timezone_info.is_china_time) {
          --myEpoch.tm_hour;
        }
      }

      auto make_time = mktime(&myEpoch);
      std::string c_time = std::ctime(&make_time);

      c_time.erase(std::remove(c_time.begin(), c_time.end(), '\n'),
                   c_time.end());

      std::cout << "\033[1;36m" << c_time
       << "\033[1;35m"
       << " UTC" << utc_sign
       << std::to_string(static_cast<uint32_t>(time_offset)) << "\033[1;36m"
       << is_daylight_saving << "."
       << "\033[0m" << std::endl
       << "[Raw hex: " << std::hex << static_cast<uint32_t>(epoch) << std::dec
       << "]";

      std::cout << std::endl;
    }

  };

};
class UseCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    uint8_t desired_device_index = stoi(args[1]);
    if (desired_device_index < device_handles_.size()) {
      handle_ = device_handles_[desired_device_index];
    } else {
      std::cout << "Invalid index!" << std::endl;
    }
    return TRunParameters{EndpointIO(), USBRequest()};
  }
};
class ClearCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{EndpointIO(), USBRequest(),[](){std::cout << "\x1B[2J\x1B[H";}};
  }
};
class ResetCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{USB_ENDPOINT_IN, USBRequest::RESET};
  }
};
class ListCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    Command::init();

    std::cout << "Usage:" << std::endl;
    std::cout << "  clear" << std::endl;
    std::cout << "  exit" << std::endl;
    std::cout << "  flashdump <direct hex address literals>" << std::endl;
    std::cout << "  in <predicate - see below>: send to usb device" << std::endl;
    std::cout << "     s:[frequence in millisecond multiples of 100ms[:max count]]:<string>: send over radio" << std::endl;
    std::cout << "       example: in s:5:HelloWorld"<< std::endl;
    std::cout << "     r: receive over radio" << std::endl;
    std::cout << "  list" << std::endl;
    std::cout << "  off" << std::endl;
    std::cout << "  on" << std::endl;
    std::cout << "  out: looping read from usb device" << std::endl;
    std::cout << "  oute: read and parse eeprom metadata" << std::endl;
    std::cout << "  use <device index>" << std::endl;
    std::cout << "  reset" << std::endl << std::endl;
    return TRunParameters{EndpointIO(), USBRequest()};
  }
};

template <typename T>
std::unique_ptr<Command> creator() {
  return std::unique_ptr<T>(new T());
}

using CommandMap = std::map<std::string, std::unique_ptr<Command>(*)()>;
CommandMap command_map {
  {"exit", &creator<ExitCommand>},
  {"flashdump", &creator<FlashDumpCommand>},
  {"in", &creator<InCommand>},
  {"out", &creator<OutCommand>},
  {"list", &creator<ListCommand>},
  {"off", &creator<OffCommand>},
  {"on", &creator<OnCommand>},
  {"oute", &creator<OuteCommand>},
  {"use", &creator<UseCommand>},
  {"clear", &creator<ClearCommand>},
  {"reset", &creator<ResetCommand>}
};
