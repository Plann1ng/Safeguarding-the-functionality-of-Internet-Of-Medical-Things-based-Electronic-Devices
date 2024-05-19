//Mimicked IoMT device with Wi-Fi without Security Framework

#include <Wi-FiNINA.h>

// Wi-Fi credentials
char ssid[] = "Your SSID";
char pass[] = "Your password";

// Server details
char server[] = "194.47.176.190"; // Replace with your server's IP address
int port = 80; // Default HTTP port

int status = WL_IDLE_STATUS;
Wi-FiClient client;

// Declare systolic and diastolic globally
int systolicPressure;
int diastolicPressure;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Connect to Wi-Fi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = Wi-Fi.begin(ssid, pass);
    delay(5000);
  }

  // Print Wi-Fi connection details
  Serial.println("Connected to Wi-Fi");
  printWi-FiStatus();
}

void loop() {
  // Check if the client can connect to the server
  if (client.connect(server, port)) {
    Serial.println("Connected to server!");
    
    // Generate random blood pressure values
    systolicPressure = random(90, 140); // Example range: 100-140 mmHg
    diastolicPressure = random(50, 90); // Example range: 60-90 mmHg
    Serial.println("Systolic Pressure: " + String(systolicPressure));
    Serial.println("Diastolic Pressure: " + String(diastolicPressure));

    // Construct the POST request body
    String postData = "systolic=" + String(systolicPressure) + "&diastolic=" + String(diastolicPressure);

    // Make a POST request to handle_post.php
    client.println("POST /handle_post.php HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(postData.length()));
    client.println();
    client.print(postData);
    client.println();

    // Wait for server response
    while (client.connected()) {
      if (client.available()) {
        Serial.println("Response from server:");
        while (client.available()) {
          Serial.write(client.read());
        }
      }
    }

    // Don't disconnect from the server, keep the connection open
    Serial.println(Wi-Fi.localIP());
    Serial.println("Request completed, waiting before sending next request...");
    delay(5000); // Delay before sending the next request
  } else {
    Serial.println("Connection to server failed");
    delay(5000); // Delay before retrying connection
  }
}

void printWi-FiStatus() {
  // Print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(Wi-Fi.SSID());

  // Print your Wi-Fi shield's IP address
  IPAddress ip = Wi-Fi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print the received signal strength
  long rssi = Wi-Fi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}
