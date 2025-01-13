#include <Arduino.h>
#include "stackble.h"

Stackble::Stackble()
    : pServer(nullptr), pCharacteristic(nullptr), deviceConnected(false), messageCallback(nullptr) {}

void Stackble::setup(const std::string &deviceName, const std::string &serviceUUID, const std::string &characteristicUUID, std::string (*callback)(const std::string &message)) {
    // Set the user-defined callback
    messageCallback = callback;

    // Initialize BLE
    BLEDevice::init(deviceName);

    // Create BLE server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(this));

    // Create BLE service
    BLEService *pService = pServer->createService(serviceUUID);

    // Create BLE characteristic
    pCharacteristic = pService->createCharacteristic(
        characteristicUUID,
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

void Stackble::response(const std::string &response) {
    if (deviceConnected) {
        pCharacteristic->setValue(response);
        pCharacteristic->notify();
        Serial.println(response.c_str());  // Fixed
    } else {
        Serial.println("No device connected to send response.");
    }
}

void Stackble::notify(const std::string &notification) {
    if (deviceConnected) {
        pCharacteristic->setValue(notification);
        pCharacteristic->notify();
        Serial.println(notification.c_str());  // Fixed
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
    std::string receivedData = pCharacteristic->getValue();
    Serial.print("Message received: ");
    Serial.println(receivedData.c_str());  // Fixed

    // Trigger the user-defined callback if set
    if (handler->messageCallback) {
        std::string response = handler->messageCallback(receivedData);
        handler->response(response);
    }
}
