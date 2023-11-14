#include <WiFi.h>

// Replace these values with your ThingSpeak channel details
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";
const char *server = "api.thingspeak.com";
String apiKey = "YOUR_API_KEY";
int channelID = YOUR_CHANNEL_ID;

// Replace this pin with the pin connected to your distance sensor
const int distanceSensorPin = A0;

void setup() {
  Serial.begin(115200);
  connectToWiFi();
}

void loop() {
  // Read distance from the sensor
  int distance = analogRead(distanceSensorPin);

  // Print distance to serial monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  // Update ThingSpeak channel with the distance value
  updateThingSpeak(distance);

  // Wait for some time before updating again (in milliseconds)
  delay(15000); // Update every 15 seconds
}

void connectToWiFi() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void updateThingSpeak(int value) {
  // Establish a connection to the ThingSpeak server
  WiFiClient client;
  if (client.connect(server, 80)) {
    // Build the HTTP POST request
    String postStr = "field1=" + String(value);
    postStr += "&api_key=" + apiKey;

    // Make a HTTP request
    client.println("POST /update HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(postStr.length()));
    client.println();
    client.println(postStr);

    // Print the server response to the serial monitor
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    // Close the connection
    client.stop();
  } else {
    // Unable to establish a connection
    Serial.println("Unable to connect to ThingSpeak");
  }
}
