// Mimicked IoMT Device with Bluetooth Low Energy Without Security Implementations

#include <ArduinoBLE.h>

BLEService serialService("1101"); // Custom service UUID
BLECharacteristic serialChar("2101", BLEWrite | BLERead | BLENotify, 20); // Custom characteristic UUID
BLECharacteristic batteryChar("2A19", BLERead | BLENotify, sizeof(uint8_t)); // Battery level characteristic UUID (standard)

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Arduino Nano 33 IoT");
  BLE.setAdvertisedService(serialService);
  serialService.addCharacteristic(serialChar);
  serialService.addCharacteristic(batteryChar);
  BLE.addService(serialService);
  
  serialChar.writeValue("Hello World"); // Initial value
  batteryChar.writeValue((uint8_t)100); // Start with 100% battery

  BLE.advertise();

  Serial.println("Bluetooth device is ready to pair");
}

void loop() {
  static unsigned long lastUpdateTime = 0; // Last time the battery was updated
  const unsigned long updateInterval = 10000; // Battery update interval (10 seconds)
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    
    while (central.connected()) {
      if (millis() - lastUpdateTime >= updateInterval) {
        updateBatteryLevel();
        lastUpdateTime = millis();
      }
      
      if (Serial.available()) {
        String input = Serial.readString();
        uint8_t buf[input.length() + 1];
        input.toCharArray((char*)buf, input.length() + 1);
        
        serialChar.writeValue(buf, input.length() + 1); // Write data to BLE characteristic
        
        Serial.print("Sent Value: ");
        Serial.println(input); // Output the value that was written
      }
      
      delay(100); 
    }
    
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void updateBatteryLevel() {
  uint8_t batteryLevel;
  batteryChar.readValue(batteryLevel);
  batteryLevel = max(0, batteryLevel - 10); // Decrease battery by 10% each interval
  batteryChar.writeValue(batteryLevel);
  Serial.print("Battery Level: ");
  Serial.println(batteryLevel);
}
