#include "stackmq.h"
#include <ArduinoJson.h> // Include ArduinoJson for JSON parsing
#include "arduino_base64.hpp" // https://github.com/dojyorin/arduino_base64, 2.0.4

Stackmq* _instance = nullptr; // Singleton instance for static callback

Stackmq::Stackmq() : _mqttClient(_secureClient) {
    _instance = this; // Assign this instance to the global pointer
}

void Stackmq::begin(const char* ssid, const char* password, const String& jwtToken, void (*callback)(String)) {
    _ssid = ssid;
    _password = password;
    _userCallback = callback;

    decodeJWT(jwtToken);

    connectWiFi();
    _secureClient.setInsecure(); // For testing without SSL certificate validation
    _mqttClient.setServer(_host.c_str(), _mqttPort);
    _mqttClient.setCallback(mqttCallback);
}

void Stackmq::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }
    if (!_mqttClient.connected()) {
        connectMQTT();
    }
    _mqttClient.loop();
}

void Stackmq::connectWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(_ssid, _password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
}

void Stackmq::connectMQTT() {
    while (!_mqttClient.connected()) {
        Serial.print("Connecting to MQTT...");
        if (_mqttClient.connect(_clientId.c_str(), _username.c_str(), _mqttPassword.c_str())) {
            Serial.println("Connected to MQTT");

            // Subscribe to the topic
            if (_mqttClient.subscribe(_subTopic.c_str())) {
                Serial.print("Subscribed to topic: ");
                Serial.println(_subTopic);
            } else {
                Serial.println("Failed to subscribe to topic!");
            }
        } else {
            Serial.print("Failed to connect. MQTT state: ");
            Serial.println(_mqttClient.state());
            delay(1000);
        }
    }
}


void Stackmq::decodeJWT(const String& jwtToken) {
    // Split JWT token using String methods
    int firstDot = jwtToken.indexOf('.');
    int secondDot = jwtToken.indexOf('.', firstDot + 1);

    String header = jwtToken.substring(0, firstDot);
    String payload = jwtToken.substring(firstDot + 1, secondDot);
    String signature = jwtToken.substring(secondDot + 1);

    // Convert payload to a C-style string
    size_t payloadLength = payload.length();
    char payloadArray[payloadLength + 1];
    payload.toCharArray(payloadArray, payloadLength + 1);

    // Decode Base64 payload using arduino_base64
    size_t decodedLength = base64::decodeLength(payloadArray); // Get the size of the decoded output
    uint8_t decodedPayload[decodedLength];
    base64::decode(payloadArray, decodedPayload); // Decode Base64 into the output buffer

    // Null-terminate the decoded payload for safe JSON parsing
    decodedPayload[decodedLength] = '\0';

    // Parse JSON payload
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, (char*)decodedPayload);

    if (error) {
        Serial.println("Failed to parse JWT payload");
        return;
    }

    JsonObject claims = doc["claims"];
    _host = String((const char*)claims["host"]);
    _mqttPort = String((const char*)claims["mqtts_port"]).toInt();
    _username = String((const char*)claims["username"]);
    _mqttPassword = String((const char*)claims["password"]);
    _subTopic = String((const char*)claims["sub_topic"]);
    _clientId = String((const char*)claims["client_id"]);

    Serial.println("JWT Decoded Successfully");
}


void Stackmq::mqttCallback(char* topic, byte* payload, unsigned int length) {
    if (_instance && _instance->_userCallback) {
        String message;
        for (unsigned int i = 0; i < length; i++) {
            message += (char)payload[i];
        }
        _instance->_userCallback(message);
    }
}
