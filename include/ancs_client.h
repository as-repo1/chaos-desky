#ifndef ANCS_CLIENT_H
#define ANCS_CLIENT_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "notification_manager.h"

extern NotificationManager notificationMgr;

struct PhoneNotificationLog {
    String lastApp = "None";
    String lastSender = "No Notifications";
    String lastMessage = "Waiting for iPhone pairing...";
    unsigned long lastTimeMs = 0;
    int totalCount = 0;
    bool connected = false;
};

class AncsNotificationClient : public BLEServerCallbacks, public BLECharacteristicCallbacks {
public:
    PhoneNotificationLog phoneLog;

    bool begin() {
        BLEDevice::init("ChaosDesky-Watch");
        pServer = BLEDevice::createServer();
        pServer->setCallbacks(this);

        // Configure BLE Security for iPhone Pairing & ANCS
        BLESecurity* pSecurity = new BLESecurity();
        pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
        pSecurity->setCapability(ESP_IO_CAP_NONE);
        pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

        // Start Advertising as a BLE Smartwatch
        pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(BLEUUID("7905F431-B5CE-4E99-A40F-4B1E122D00D0")); // ANCS Service UUID
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections
        pAdvertising->setMinPreferred(0x12);
        BLEDevice::startAdvertising();

        Serial.println("✅ Smartwatch BLE ANCS Notification Receiver Started! Device: ChaosDesky-Watch");
        return true;
    }

    void onConnect(BLEServer* pServer) override {
        phoneLog.connected = true;
        Serial.println("📲 iPhone Connected over BLE!");
        notificationMgr.trigger("iPhone Connected", "Smartwatch BLE Sync Active", NOTIF_INFO, NOTIF_TARGET_BOTH, 6);
    }

    void onDisconnect(BLEServer* pServer) override {
        phoneLog.connected = false;
        Serial.println("📲 iPhone Disconnected from BLE.");
        BLEDevice::startAdvertising();
    }

    void simulateNotification(const String& sender, const String& text, NotificationCategory cat = NOTIF_MESSAGE) {
        phoneLog.lastApp = (cat == NOTIF_CALL) ? "Phone Call" : "WhatsApp";
        phoneLog.lastSender = sender;
        phoneLog.lastMessage = text;
        phoneLog.lastTimeMs = millis();
        phoneLog.totalCount++;

        notificationMgr.trigger(sender, text, cat, NOTIF_TARGET_BOTH, 10);
    }

private:
    BLEServer* pServer = nullptr;
    BLEAdvertising* pAdvertising = nullptr;
};

#endif // ANCS_CLIENT_H
