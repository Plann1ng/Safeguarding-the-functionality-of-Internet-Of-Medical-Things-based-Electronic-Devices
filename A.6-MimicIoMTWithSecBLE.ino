#include <ArduinoBLE.h>
#include <stdlib.h> // Include standard library for random number generation

BLEService serialService("1101");  // BLE service UUID
BLECharacteristic serialChar("2101", BLERead | BLENotify, 512);  // BLE characteristic UUID

// Function Prototypes
uint32_t simpleHash(const String& mac);
bool isWhitelisted(uint32_t addressHash);
bool shouldDisconnect(uint32_t addressHash);
String xorEncryptDecrypt(String input);

// Encryption key
const char* encryptionKey = "secretkey";
int keyLength = strlen(encryptionKey);

// Whitelist and device record structures
const uint32_t whitelistedHashes[] = {simpleHash("57:01:8b:be:c3:16")};
const int whitelistSize = 1;
struct DeviceRecord {
  uint32_t addressHash;
  uint8_t attempts;
  unsigned long blacklistTimestamp;
};
DeviceRecord deviceRecords[10];
int numRecords = 0;
const uint8_t maxAttempts = 3;
const unsigned long blacklistDuration = 1800000;

void setup() {
  Serial.begin(9600);
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Arduino Nano 33 IoT");
  BLE.setAdvertisedService(serialService);
  serialService.addCharacteristic(serialChar);
  BLE.addService(serialService);

  // Write encrypted initial value
  String initialMsg = "Hello World";
  serialChar.writeValue(xorEncryptDecrypt(initialMsg).c_str());
  
  BLE.advertise();
  BLE.setAdvertisingInterval(1600);  // 1 second interval
  Serial.println("Bluetooth device is ready to pair");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    uint32_t centralHash = simpleHash(central.address());

    if (isWhitelisted(centralHash)) {
      Serial.println("Device is whitelisted. Connection allowed.");
      while (central.connected()) {
        // Generate random systolic and diastolic values
        int systolic = random(80, 131);  // Random systolic between 80 and 130
        int diastolic = random(100, 171); // Random diastolic between 100 and 170
        String data = "Systolic: " + String(systolic) + " Diastolic: " + String(diastolic);
        // Encrypt and send data
        serialChar.writeValue(xorEncryptDecrypt(data).c_str());
        delay(1000); // Send new data every second
      }
    } else {
      Serial.println("Device not in whitelist.");
      if (shouldDisconnect(centralHash)) {
        Serial.println("Device is temporarily blacklisted. Disconnecting.");
        central.disconnect();
      }
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

uint32_t simpleHash(const String& mac) {
  uint32_t hash = 0;
  for (int i = 0; i < mac.length(); i++) {
    if (mac[i] != ':') {
      hash = hash * 16 + (mac[i] >= 'a' ? (mac[i] - 'a' + 10) : (mac[i] - '0'));
    }
  }
  return hash;
}

bool isWhitelisted(uint32_t addressHash) {
  for (int i = 0; i < whitelistSize; i++) {
    if (whitelistedHashes[i] == addressHash) {
      return true;
    }
  }
  return false;
}

bool shouldDisconnect(uint32_t addressHash) {
  unsigned long currentTime = millis();
  for (int i = 0; i < numRecords; i++) {
    if (deviceRecords[i].addressHash == addressHash) {
      if (currentTime - deviceRecords[i].blacklistTimestamp < blacklistDuration) {
        return true;
      }
      deviceRecords[i].attempts++;
      if (deviceRecords[i].attempts > maxAttempts) {
        deviceRecords[i].blacklistTimestamp = currentTime;
        return true;
      }
      return false;
    }
  }
  if (numRecords < 10) {
    deviceRecords[numRecords++] = {addressHash, 1, 0};  // Add new record
  }
  return false;
}

String xorEncryptDecrypt(String input) {
  String output = input;
  for (int i = 0; i < input.length(); i++) {
    output[i] = input[i] ^ encryptionKey[i % keyLength];
  }
  return output;
}
