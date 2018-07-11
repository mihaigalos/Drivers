#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <exception>


void onExit(std::vector<usb_dev_handle *> &handles);
auto getUsbHandles() -> std::vector<usb_dev_handle *>;

using EndpointIO = int;
struct TRunParameters{
  EndpointIO endpoint;
  USBRequest request;
  std::function<void()> postAction;
}; //std::tuple<EndpointIO, USBRequest>;

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
      result = usb_control_msg(handle_, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
        static_cast<int>(parameters.endpoint),
        static_cast<int>(parameters.request),
        0, 0,
        (char *)buffer_, sizeof(buffer_), 5000);
    }

    if(parameters.postAction) parameters.postAction();

    return result;

  }
protected:
   virtual TRunParameters run(std::vector<std::string>& args) = 0;
   static usb_dev_handle * handle_;
   static std::vector<usb_dev_handle *> device_handles_;
   static char buffer_[254];
private:
   static std::vector<std::string> empty_vector_;

};

std::vector<std::string> Command::empty_vector_;
usb_dev_handle * Command::handle_ {nullptr};
char Command::buffer_[254];
std::vector<usb_dev_handle *> Command::device_handles_;


class ExitCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{EndpointIO(), USBRequest()};
  }
};
class FlashDumpCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{EndpointIO(), USBRequest()};
  }
};
class InCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{EndpointIO(), USBRequest()};
  }
};
class OutCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{EndpointIO(), USBRequest(), Utils::looping_dump};
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
        usb_control_msg(handle_, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
          USB_ENDPOINT_IN,
          static_cast<int>(USBRequest::DATA_OUT), 0, 0,
          (char *)buffer_, sizeof(buffer_), 5000);
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

class OffCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    std::cout<<"Off!"<<std::endl;
    return TRunParameters{USB_ENDPOINT_IN, USBRequest::LED_OFF};
  }
};
class OnCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
     std::cout<<"On!"<<std::endl;
    return TRunParameters{USB_ENDPOINT_IN, USBRequest::LED_ON};
  }
};
class OuteCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{EndpointIO(), USBRequest()};
  }
};
class UseCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    init();
    return TRunParameters{EndpointIO(), USBRequest()};
  }
private:
  std::vector<usb_dev_handle *> device_handles;
};
class ResetCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    return TRunParameters{EndpointIO(), USBRequest()};
  }
};
class ListCommand : public Command{
public:
  TRunParameters run(std::vector<std::string>& args) override {
    std::cout << "Usage:" << std::endl;
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
  {"reset", &creator<ResetCommand>}
};
