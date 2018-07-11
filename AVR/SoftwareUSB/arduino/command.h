#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <exception>


void onExit(std::vector<usb_dev_handle *> &handles);
auto getUsbHandles() -> std::vector<usb_dev_handle *>;

class Command {
public:
  using EndpointIO = int;

  static void init(){
    onExit(device_handles_);
    Command::device_handles_ = getUsbHandles();
    Command::handle_ = device_handles_.at(0);
  }

  auto execute(std::vector<std::string>& args = empty_vector_) -> int{
    auto result = 0;
    std::tuple<EndpointIO, USBRequest> parameters = run (args);
    auto endpoint = std::get<0>(parameters);
    auto request = std::get<1>(parameters);

    // This does not work, probably the handle has changed..
    // result = usb_control_msg(handle_, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
    //                                        USB_ENDPOINT_IN,
    //                            static_cast<int>(USBRequest::LED_OFF), 0, 0,
    //                            (char *)buffer_, sizeof(buffer_), 5000);


    if(USBRequest::Unknown != request){

      std::cout<<"<0>: "<<static_cast<int>(std::get<0>(parameters))<<std::endl;
      std::cout<<"<1>: "<<static_cast<int>(std::get<1>(parameters))<<std::endl;
      std::cout<<"<h>: "<<std::hex<<"0x"<<reinterpret_cast<long long>(handle_)<<std::dec<<std::endl;


      result = usb_control_msg(handle_, USB_TYPE_VENDOR | USB_RECIP_DEVICE |
        static_cast<int>(endpoint),
        static_cast<int>(request),
        0, 0,
        (char *)buffer_, sizeof(buffer_), 5000);

        std::cout<< "result: "<<result<<std::endl;
    }
    return result;

  }
protected:
   virtual std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) = 0;
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
  std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) override {
    return std::tuple<EndpointIO, USBRequest>{EndpointIO(), USBRequest()};
  }
};
class FlashDumpCommand : public Command{
public:
  std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) override {
    return std::tuple<EndpointIO, USBRequest>{EndpointIO(), USBRequest()};
  }
};
class InCommand : public Command{
public:
  std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) override {
    return std::tuple<EndpointIO, USBRequest>{EndpointIO(), USBRequest()};
  }
};
class OutCommand : public Command{
public:
  std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) override {
    return std::tuple<EndpointIO, USBRequest>{EndpointIO(), USBRequest()};
  }
};

class OffCommand : public Command{
public:
  std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) override {
    std::cout<<"Off!"<<std::endl;
    return std::tuple<EndpointIO, USBRequest>{USB_ENDPOINT_IN, USBRequest::LED_OFF};
  }
};
class OnCommand : public Command{
public:
  std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) override {
     std::cout<<"On!"<<std::endl;
    return std::tuple<EndpointIO, USBRequest>{USB_ENDPOINT_IN, USBRequest::LED_ON};
  }
};
class OuteCommand : public Command{
public:
  std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) override {
    return std::tuple<EndpointIO, USBRequest>{EndpointIO(), USBRequest()};
  }
};
class UseCommand : public Command{
public:
  std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) override {

    // unsigned int desired_device_index = stoi(args[1]);
    init();
      // if (desired_device_index < device_handles_.size()) {
      //   handle_ = device_handles_[desired_device_index];
      // } else {
      //   std::cout << "Invalid index!" << std::endl;
      // }

    return std::tuple<EndpointIO, USBRequest>{EndpointIO(), USBRequest()};
  }
private:
  std::vector<usb_dev_handle *> device_handles;
};
class ResetCommand : public Command{
public:
  std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) override {
    return std::tuple<EndpointIO, USBRequest>{EndpointIO(), USBRequest()};
  }
};
class ListCommand : public Command{
public:
  std::tuple<EndpointIO, USBRequest> run(std::vector<std::string>& args) override {
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
    return std::tuple<EndpointIO, USBRequest>{EndpointIO(), USBRequest()};
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
