# Injection logic
from bluepy.btle import Peripheral, UUID

class BLEConnection:
    def __init__(self, addr):
        self.device_address = addr
        self.peripheral = None

    def connect(self):
        try:
            print(f"Connecting to {self.device_address}")
            self.peripheral = Peripheral(self.device_address, addrType="public")
            print("Connected successfully.")
        except Exception as e:
            print(f"Connection failed: {e}")

    def write_data(self):
        try:
            service_uuid = UUID("00001101-0000-1000-8000-00805f9b34fb")
            char_uuid = UUID("00002101-0000-1000-8000-00805f9b34fb")
            service = self.peripheral.getServiceByUUID(service_uuid)
            char = service.getCharacteristics(char_uuid)[0]
            data = bytes([0x00, 0x64, 0x00, 0x00, 0x01, 0xf4])  # Modify as per the actual required format
            char.write(data, withResponse=True)
            print("Data written to the device")
        except Exception as e:
            print(f"Failed to write data: {e}")

    def disconnect(self):
        if self.peripheral:
            self.peripheral.disconnect()
            print("Disconnected")

# Usage
device_address = 'bd:54:0a:08:2d:39'  # Your device's MAC address
ble_manager = BLEConnection(device_address)
ble_manager.connect()
ble_manager.write_data()
ble_manager.disconnect()
