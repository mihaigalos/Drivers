// code adapted from http://www.johngineer.com/blog/?p=455
#include <Arduino.h>

#define CAPTURE_SIZE 5

typedef void (*ObserverCallback)();

class AbstractSniffer {

protected:
  static uint8_t captured_data_[CAPTURE_SIZE];
  static uint8_t captured_data_count_;
  bool captured;
  bool dumped;
  static ObserverCallback observer_;

public:
  virtual void run() = 0;
  virtual void init() = 0;
  void reset() {
    memset(captured_data_, 0, sizeof(captured_data_));
    captured_data_count_ = 0;
  }

  const uint8_t &getData(int pos) { return captured_data_[pos]; }
  const uint8_t &getCount() { return captured_data_count_; }
};
