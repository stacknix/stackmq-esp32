#include "stackmq.h"

// Create an instance of Stackmq
Stackmq stackmq;

// Callback function to handle received MQTT messages
void callback(String message) {
    Serial.println("Received message: " + message);
}

void setup() {
    Serial.begin(115200);

    // WiFi credentials
    const char* ssid = "YOUR_WIFI_SSID";
    const char* password = "YOUR_WIFI_PASSWORD";

    // JWT token as String
    String jwtToken = "YOUR_JWT_TOKEN";

    // Initialize the Stackmq library
    stackmq.begin(ssid, password, jwtToken, callback);
}

void loop() {
    stackmq.loop(); // Continuously handle MQTT events
}
