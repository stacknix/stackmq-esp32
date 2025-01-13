#ifndef STACKBLE_H
#define STACKBLE_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class Stackble {
public:
    // Constructor
    Stackble();

    // Setup BLE with parameters
    void setup(const std::string &deviceName, const std::string &serviceUUID, const std::string &characteristicUUID, std::string (*callback)(const std::string &message));

    // Send a response to the BLE client
    void response(const std::string &response);

    // Notify the connected BLE client with any string
    void notify(const std::string &notification);

    // Check if a device is connected
    bool isDeviceConnected() const;

private:
    BLEServer *pServer;
    BLECharacteristic *pCharacteristic;
    bool deviceConnected;

    // Callback function for message handling
    std::string (*messageCallback)(const std::string &message);

    // BLE Callbacks
    class ServerCallbacks : public BLEServerCallbacks {
        Stackble *handler;
    public:
        ServerCallbacks(Stackble *handler) : handler(handler) {}
        void onConnect(BLEServer *pServer) override;
        void onDisconnect(BLEServer *pServer) override;
    };

    class CharacteristicCallbacks : public BLECharacteristicCallbacks {
        Stackble *handler;
    public:
        CharacteristicCallbacks(Stackble *handler) : handler(handler) {}
        void onWrite(BLECharacteristic *pCharacteristic) override;
    };
};

#endif
