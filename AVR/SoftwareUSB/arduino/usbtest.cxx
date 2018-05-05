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

#include  <ctime>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <ctime>

#include <cmath>
#include <functional>

#include "i_usbRequest.h"

// used to get descriptor strings for device identification 
static int usbGetDescriptorString(usb_dev_handle *dev, int index, int langid,
		char *buf, int buflen) {
	char buffer[256];
	int rval, i;

	// make standard request GET_DESCRIPTOR, type string and given index 
	// (e.g. dev->iProduct)
	rval = usb_control_msg(dev,
			USB_TYPE_STANDARD | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
			USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) + index, langid,
			buffer, sizeof(buffer), 1000);

	if (rval < 0) // error
		return rval;

	// rval should be bytes read, but buffer[0] contains the actual response size
	if ((unsigned char) buffer[0] < rval)
		rval = (unsigned char) buffer[0]; // string is shorter than bytes read

	if (buffer[1] != USB_DT_STRING) // second byte is the data type
		return 0; // invalid return type

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

static usb_dev_handle * usbOpenDevice(int vendor, char *vendorName, int product,
		char *productName) {
	struct usb_bus *bus;
	struct usb_device *dev;
	char devVendor[256], devProduct[256];

	usb_dev_handle * handle = NULL;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (bus = usb_get_busses(); bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor != vendor
					|| dev->descriptor.idProduct != product)
				continue;

			// we need to open the device in order to query strings 
			if (!(handle = usb_open(dev))) {
				fprintf(stderr, "Warning: cannot open USB device: %s\n",
						usb_strerror());
				continue;
			}

			// get vendor name 
			if (usbGetDescriptorString(handle, dev->descriptor.iManufacturer,
					0x0409, devVendor, sizeof(devVendor)) < 0) {
				fprintf(stderr,
						"Warning: cannot query manufacturer for device: %s\n",
						usb_strerror());
				usb_close(handle);
				continue;
			}

			// get product name 
			if (usbGetDescriptorString(handle, dev->descriptor.iProduct, 0x0409,
					devProduct, sizeof(devVendor)) < 0) {
				fprintf(stderr,
						"Warning: cannot query product for device: %s\n",
						usb_strerror());
				usb_close(handle);
				continue;
			}

			fprintf(stdout, "Found vendor: %s\n", devVendor);
			fprintf(stdout, "Found product: %s\n\n", devProduct);

			if (/*strcmp(devVendor, vendorName) == 0 && */
			strcmp(devProduct, productName) == 0)
				return handle;
			else {
				usb_close(handle);
				fprintf(stderr,
						"Cannot find usb device from : \nVendor: %s\nProduct: %s\n\n",
						vendorName, productName);
			}
		}
	}

	return NULL;
}

void printReceivedBytes(uint16_t start_address, uint16_t nBytes, char buffer[],
		std::string separator = "", bool print_bytecount = true) {
	if (print_bytecount)
		std::cout << "Got " << nBytes << " bytes: " << std::endl;
	const uint16_t kIntelHexByteCount = 0x10;
	const uint16_t kAddressIncrement = 0x10;

	uint16_t crc = kIntelHexByteCount;
	uint16_t start_offset =
			static_cast<uint16_t>(static_cast<double>(start_address)
					/ static_cast<double>(columnCount)) * columnCount;

	crc += start_offset >> 8;
	crc += static_cast<uint8_t>(start_offset);

	std::cout << std::uppercase << std::hex;

	auto printCrc = [&](uint16_t iteration_mod_column_count) {
		crc = -crc;
		crc = static_cast<uint8_t>(crc);

		if (crc < 16)
		std::cout << "0";

		std::cout << crc << std::endl;

		crc = kAddressIncrement;

		uint16_t foo = iteration_mod_column_count + start_offset;

		crc += foo>>8;
		crc += static_cast<uint8_t>(foo);

	};

	auto printNumberOfBytes =
			[&] (uint16_t i) {
				uint16_t div16 = static_cast<uint16_t>(static_cast<double>(i+start_address) / static_cast<double>(kAddressIncrement))*kAddressIncrement;

				std::cout << ":10";

				if (!(0xF000 & div16))
				{
					std::cout << "0";
					if (!(0x0F00 & div16))
					{
						std::cout << "0";
						if (!(0x00F0 & div16)) {
							std::cout << "0";
						}
					}
				}

				std::cout << div16;
			};

	auto printRecordType = [&] {
		std::cout<<"00";
	};

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
			std::cout << "0";
		std::cout << value;
		std::cout << separator;
//		std::cout << std::endl << "    crc:" << std::hex << crc << std::endl;

	}

	printCrc(1);

	std::cout << std::dec << std::nouppercase;
}

int main(int argc, char **argv) {
	usb_dev_handle *handle = NULL;
	int nBytes = 0;
	char buffer[254];

	if (argc < 2) {
		printf("Usage:\n");
		printf("usbtext.exe on\n");
		printf("usbtext.exe off\n");
		printf("usbtext.exe out\n");
		printf("usbtext.exe in <string>\n");
		printf("usbtext.exe flashdump <direct hex address literals>\n");
    printf("usbtext.exe reset\n");
		exit(1);
	}
	auto start = std::chrono::high_resolution_clock::now();
	handle = usbOpenDevice(0x16C0, const_cast<char*>(std::string {
			"Galos Industries" }.c_str()), 0x05DC,
			const_cast<char*>(std::string { "DotPhat" }.c_str()));

	if (handle == NULL) {
		fprintf(stderr, "Could not find USB device!\n");
		exit(1);
	}

	if (strcmp(argv[1], "on") == 0) {
		nBytes = usb_control_msg(handle,
				USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
				static_cast<int>(USBRequest::LED_ON), 0, 0, (char *) buffer,
				sizeof(buffer), 5000);
	} else if (strcmp(argv[1], "off") == 0) {
		nBytes = usb_control_msg(handle,
				USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
				static_cast<int>(USBRequest::LED_OFF), 0, 0, (char *) buffer,
				sizeof(buffer), 5000);
	} else if (strcmp(argv[1], "out") == 0) {

		nBytes = usb_control_msg(handle,
				USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
				static_cast<int>(USBRequest::DATA_OUT), 0, 0, (char *) buffer,
				sizeof(buffer), 5000);
		printf("Got %d bytes: %s\n", nBytes, buffer);
		//printReceivedBytes(0, nBytes, buffer);
	} else if (strcmp(argv[1], "inOOOOOLD") == 0) {
		nBytes = usb_control_msg(handle,
				USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
				static_cast<int>(USBRequest::DATA_WRITE), 'T' + ('E' << 8),
				'S' + ('T' << 8), (char *) buffer, sizeof(buffer), 5000);
	} else if (strcmp(argv[1], "in") == 0) {
    
    char buffer2 [] = "This is an awesome test.";
    
		nBytes = usb_control_msg(handle,
				USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
				static_cast<int>(USBRequest::DATA_WRITE), 0,
				0, (char *) buffer2, strlen(buffer2)+1, 5000);
	} else if (strcmp(argv[1], "flashdump") == 0) {

		if (argc > 2) {
			nBytes = usb_control_msg(handle,
					USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
					static_cast<int>(USBRequest::FLASH_DUMP_FROM_ADDRESS), 0, 0,
					argv[2], strlen(argv[2]) + 1, 5000);
		} else {
			char address_hex[6] = "0";
			constexpr double atmega328p_flash_size = 32 * 1024;
			constexpr uint16_t repeat_count = static_cast<uint16_t>(std::ceil(
					atmega328p_flash_size / static_cast<double>(kBufferSize)));

			for (uint16_t i = 0; i < repeat_count; ++i) {
				uint16_t offset = i * kBufferSize;
				sprintf(address_hex, "%x", offset);

				nBytes = usb_control_msg(handle,
						USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
						static_cast<int>(USBRequest::FLASH_DUMP_FROM_ADDRESS),
						0, 0, address_hex, strlen(address_hex) + 1, 5000);

				nBytes = usb_control_msg(handle,
						USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
						static_cast<int>(USBRequest::DATA_OUT), 0, 0,
						(char *) buffer, sizeof(buffer), 5000);
				printReceivedBytes(offset, nBytes, buffer, "", false);

			}

		}

	} else if (strcmp(argv[1], "reset") == 0){
    nBytes = usb_control_msg(handle,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
        static_cast<int>(USBRequest::RESET), 0, 0, (char *) buffer,
        sizeof(buffer), 5000);
  }

	if (nBytes < 0)
		fprintf(stderr, "USB error: %s\n", usb_strerror());

	usb_close(handle);

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long microseconds = std::chrono::duration_cast
			< std::chrono::milliseconds > (elapsed).count();
	std::cout << "CPU time used: " << microseconds << " ms\n";

	return 0;
}
