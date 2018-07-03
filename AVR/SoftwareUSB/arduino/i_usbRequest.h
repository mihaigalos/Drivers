#pragma once

constexpr uint16_t columnCount { 15 };
constexpr uint16_t bytesPercolumn { 16 };

constexpr uint16_t kBufferSize { bytesPercolumn * columnCount };

enum class USBRequest {
	Unknown, LED_OFF, LED_ON, DATA_OUT, DATA_WRITE, FLASH_DUMP_FROM_ADDRESS, RESET
};

extern uint8_t buffer[kBufferSize];
extern uint8_t dataReceived, dataLength; // for USB_DATA_IN
