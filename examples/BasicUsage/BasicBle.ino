#include <stackble.h>

// Create a Stackble instance
Stackble stackble;

// Callback function for handling received messages
std::string callbackBle(const std::string &message) {
    Serial.print("Received: ");
    Serial.println(message.c_str());

    // Return a response to be sent to the client
    return "Echo: " + message;
}

void setup() {
    Serial.begin(115200);

    // Initialize BLE with parameters
    stackble.setup("ESP32_Stackble", "4fafc201-1fb5-459e-8fcc-c5c9c331914b", "beb5483e-36e1-4688-b7f5-ea07361b26a8", callbackBle);
}

void loop() {
    // Example: Notify battery status every 5 seconds
    static unsigned long lastNotifyTime = 0;
    if (millis() - lastNotifyTime > 5000) {
        lastNotifyTime = millis();

        if (stackble.isDeviceConnected()) {
            stackble.notify("Battery: 85%");
        } else {
            Serial.println("No connected device to notify.");
        }
    }
}
