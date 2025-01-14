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
    void setup(const String &deviceName, const String &serviceUUID, const String &characteristicUUID, String (*callback)(String message));

    // Send a response to the BLE client
    void response(const String &response);

    // Notify the connected BLE client with any string
    void notify(const String &notification);

    // Check if a device is connected
    bool isDeviceConnected() const;

private:
    BLEServer *pServer;
    BLECharacteristic *pCharacteristic;
    bool deviceConnected;

    // Callback function for message handling
    String (*messageCallback)(String message);

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
