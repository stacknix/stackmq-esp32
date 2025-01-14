#ifndef STACKMQ_H
#define STACKMQ_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

class Stackmq {
public:
    Stackmq();

    void setup(const char* ssid, const char* password, const String& jwtToken, void (*callback)(String));
    void loop();

private:
    const char* _ssid;
    const char* _password;

    String _host;
    int _mqttPort;
    String _username;
    String _mqttPassword;
    String _subTopic;
    String _clientId;

    WiFiClientSecure _secureClient;
    PubSubClient _mqttClient;

    void (*_userCallback)(String); // User's callback function

    void connectWiFi();
    void connectMQTT();

    // Internal helper functions
    void decodeJWT(const String& jwtToken);
    static void mqttCallback(char* topic, byte* payload, unsigned int length);
};

#endif
