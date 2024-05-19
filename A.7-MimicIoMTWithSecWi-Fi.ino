//Mimicked IoMT Device with Wi-Fi Secuirty Framework
#include <SPI.h>
#include <Wi-FiNINA.h>

// Network credentials
char ssid[] = "ASUS_A0";         // your network SSID (name)
char pass[] = "delivery_9265";   // your network password

int status = WL_IDLE_STATUS;
Wi-FiSSLClient client;

int systolicPressure;
int diastolicPressure;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Check for the presence of the Wi-Fi module
  if (Wi-Fi.status() == WL_NO_MODULE) {
    Serial.println("Communication with Wi-Fi module failed!");
    // Use an infinite loop to halt further execution if the module is not found
    while (true);
  }

  // Attempt to connect to the Wi-Fi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = Wi-Fi.begin(ssid, pass);
    delay(10000); // Wait 10 seconds for connection
  }
  Serial.println("Connected to Wi-Fi.");
}

void loop() {
  Serial.println("Starting connection to server...");
  if (client.connectSSL("cscloud6-190.lnu.se", 443)) {
    Serial.println("Connected to server");

    // Generate random pressure readings
    systolicPressure = random(90, 140); // Example range: 100-140 mmHg
    diastolicPressure = random(50, 90); // Example range: 60-90 mmHg
    Serial.println("Systolic Pressure: " + String(systolicPressure));
    Serial.println("Diastolic Pressure: " + String(diastolicPressure));

    // Prepare and send the POST data
    String postData = "systolic=" + String(systolicPressure) + "&diastolic=" + String(diastolicPressure);
    client.println("POST /handle_post.php HTTP/1.1");
    client.println("Host: cscloud6-190.lnu.se");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(postData.length()));
    client.println();
    client.print(postData);
    client.println();

    // Read and print the server response
    while (client.connected()) {
      if (client.available()) {
        Serial.println("Response from server:");
        while (client.available()) {
          char c = client.read();
          Serial.write(c);
        }
      }
    }

    Serial.println("Request completed, waiting before sending next request...");
    delay(5000); // Wait before sending the next request
  } else {
    Serial.println("Connection failed");
  }

  // Disconnect from the server
  if (!client.connected()) {
    Serial.println("Disconnecting from server.");
    client.stop();
  }
}
