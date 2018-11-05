# Read() : https://stackoverflow.com/questions/32807781/ble-subscribe-to-notification-using-gatttool-or-bluepy
# Gread Android app for debugging : LightBlue Explorer.

import bluepy.btle as btle
import binascii
import struct

class MyDelegate(btle.DefaultDelegate):
    def __init__(self):
        btle.DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
        print("A notification was received: %s" %data)

class BluetoothModule:
    def __init__(self, address):
        self.peripheral = btle.Peripheral(address)

    def write(self, string_to_write):
        service = self.peripheral.getServiceByUUID("0000ffe0-0000-1000-8000-00805f9b34fb")
        characteristic = service.getCharacteristics()[2]
        characteristic.write(bytes(string_to_write), True)

    def read(self):
        self.peripheral.setDelegate( MyDelegate() )
        svc = self.peripheral.getServiceByUUID( "0000ffe0-0000-1000-8000-00805f9b34fb")
        ch = svc.getCharacteristics()[0]
        print(ch.valHandle)
        self.peripheral.writeCharacteristic(ch.valHandle+1, "\x01\x00")

        while True:
            if self.peripheral.waitForNotifications(1.0):
                continue
            print("Waiting...")

jdy16 = BluetoothModule("3C:A5:39:90:BB:B1")
jdy16.write("Foobar")
jdy16.read()
