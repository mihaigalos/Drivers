#pragma once

constexpr uint16_t columnCount { 30 };
constexpr uint16_t bytesPercolumn { 8 };

constexpr uint16_t kBufferSize { bytesPercolumn * columnCount };

enum class USBRequest {
	LED_OFF, LED_ON, DATA_OUT, DATA_WRITE, FLASH_DUMP_FROM_ADDRESS
};
