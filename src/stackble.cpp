#include <Arduino.h>
#include "stackble.h"

Stackble::Stackble()
    : pServer(nullptr), pCharacteristic(nullptr), deviceConnected(false), messageCallback(nullptr) {}

void Stackble::setup(const String &deviceName, const String &serviceUUID, const String &characteristicUUID, String (*callback)(String message)) {
    // Set the user-defined callback
    messageCallback = callback;

    // Initialize BLE
    BLEDevice::init(deviceName.c_str());

    // Create BLE server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(this));

    // Create BLE service
    BLEService *pService = pServer->createService(serviceUUID.c_str());

    // Create BLE characteristic
    pCharacteristic = pService->createCharacteristic(
        characteristicUUID.c_str(),
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // Add descriptor and set callbacks
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new CharacteristicCallbacks(this));

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(pCharacteristic->getUUID());
    pAdvertising->start();
    Serial.println("BLE advertising started...");
}

void Stackble::response(const String &response) {
    if (deviceConnected) {
        pCharacteristic->setValue(response.c_str());
        pCharacteristic->notify();
        Serial.println(response);
    } else {
        Serial.println("No device connected to send response.");
    }
}

void Stackble::notify(const String &notification) {
    if (deviceConnected) {
        pCharacteristic->setValue(notification.c_str());
        pCharacteristic->notify();
        Serial.println(notification);
    } else {
        Serial.println("No device connected to send notification.");
    }
}

bool Stackble::isDeviceConnected() const {
    return deviceConnected;
}

// BLE Server Callbacks
void Stackble::ServerCallbacks::onConnect(BLEServer *pServer) {
    handler->deviceConnected = true;
    Serial.println("Client connected.");
}

void Stackble::ServerCallbacks::onDisconnect(BLEServer *pServer) {
    handler->deviceConnected = false;
    Serial.println("Client disconnected.");
}

// BLE Characteristic Callbacks
void Stackble::CharacteristicCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    String receivedData = pCharacteristic->getValue().c_str();
    Serial.print("Message received: ");
    Serial.println(receivedData);

    // Trigger the user-defined callback if set
    if (handler->messageCallback) {
        String response = handler->messageCallback(receivedData);
        handler->response(response);
    }
}
