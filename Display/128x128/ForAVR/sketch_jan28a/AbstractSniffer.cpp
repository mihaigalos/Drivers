#include "AbstractSniffer.h"

ObserverCallback AbstractSniffer::observer_;

uint8_t AbstractSniffer::captured_data_[CAPTURE_SIZE];
uint8_t AbstractSniffer::captured_data_count_;
