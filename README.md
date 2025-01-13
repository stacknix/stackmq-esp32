# StackMQ

StackMQ is an Arduino library that simplifies MQTT communication with a custom MQTT broker. It supports JWT-based authentication, secure connections, and automated topic subscription, making it ideal for IoT applications.

---

## Features

- **JWT Authentication**: Handles authentication with JWT tokens for your custom MQTT broker.
- **Secure Connections**: Supports TLS/SSL for secure communication.
- **Lightweight and Fast**: Minimal overhead for embedded systems.
- **Easy to Use**: Simple API for connecting, subscribing, and receiving messages.

---

## Installation

### Using the Arduino Library Manager
1. Open the Arduino IDE.
2. Go to **Tools > Manage Libraries...**.
3. Search for `StackMQ` and click **Install**.

---

## Dependencies

StackMQ relies on the following libraries, which will be installed automatically when you install StackMQ via the Arduino Library Manager:

1. **[PubSubClient](https://github.com/knolleary/pubsubclient)**:
    - MQTT client library for Arduino.
    - Used for connecting to the MQTT broker and managing subscriptions.

2. **[ArduinoJson](https://github.com/bblanchon/ArduinoJson)**:
    - For parsing JSON data, including JWT payloads.

3. **[arduino_base64](https://github.com/dojyorin/arduino_base64)**:
    - Used for decoding Base64-encoded JWT tokens.

---

## Getting Started

### Example: Connecting to a Custom MQTT Broker

```cpp
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
