#ifndef BLE_WIFI_PROVISION_H
#define BLE_WIFI_PROVISION_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
#include "config_manager.h"
#include "notification_manager.h"
#include "display_tft.h"

extern ConfigManager configMgr;
extern NotificationManager notificationMgr;
extern TftDisplayManager tftMgr;

// Nordic UART Service (NUS) Standard UUIDs for broad mobile/PC app compatibility
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class BleWifiProvisioner : public BLEServerCallbacks, public BLECharacteristicCallbacks {
public:
    BLEServer* pServer = nullptr;
    BLECharacteristic* pTxCharacteristic = nullptr;
    bool deviceConnected = false;
    bool oldDeviceConnected = false;
    bool pendingReconnect = false;
    unsigned long reconnectTriggerMs = 0;

    void begin() {
        if (!configMgr.config.featureBleEnabled) {
            Serial.println("📡 BLE Wi-Fi Provisioning disabled by feature toggle.");
            return;
        }

        BLEDevice::init("chaos-desky-wifi");
        pServer = BLEDevice::createServer();
        pServer->setCallbacks(this);

        BLEService* pService = pServer->createService(SERVICE_UUID);

        pTxCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID_TX,
            BLECharacteristic::PROPERTY_NOTIFY
        );
        pTxCharacteristic->addDescriptor(new BLE2902());

        BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID_RX,
            BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
        );
        pRxCharacteristic->setCallbacks(this);

        pService->start();

        BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06);
        pAdvertising->setMaxPreferred(0x12);
        BLEDevice::startAdvertising();

        Serial.println("📡 BLE Wi-Fi Provisioning Server Started (Name: chaos-desky-wifi)");
    }

    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("📲 BLE Provisioner Connected!");
        notificationMgr.trigger("BLE Connected", "Ready for Wi-Fi Config", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 5);
        
        sendReply("\r\n============================\r\n🚀 CHAOS DESKY WIFI SETUP\r\nSend credentials as:\r\nSSID,PASSWORD\r\nor SSID\\nPASSWORD\r\n============================\r\n");
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        Serial.println("📲 BLE Provisioner Disconnected.");
    }

    void update() {
        if (!configMgr.config.featureBleEnabled) return;

        if (!deviceConnected && oldDeviceConnected) {
            delay(500); // Allow BLE stack to reset
            if (WiFi.status() != WL_CONNECTED) {
                pServer->startAdvertising();
                Serial.println("📡 BLE Re-advertising for WiFi config...");
            }
            oldDeviceConnected = deviceConnected;
        }
        if (deviceConnected && !oldDeviceConnected) {
            oldDeviceConnected = deviceConnected;
        }

        // Check if a reconnection was triggered via BLE RX
        if (pendingReconnect && (millis() - reconnectTriggerMs >= 1500)) {
            pendingReconnect = false;
            Serial.println("🔄 Reconnecting to new Wi-Fi network: " + configMgr.config.wifiSsid);
            WiFi.disconnect();
            WiFi.begin(configMgr.config.wifiSsid.c_str(), configMgr.config.wifiPass.c_str());
            notificationMgr.trigger("WiFi Connecting", "Joining: " + configMgr.config.wifiSsid, NOTIF_INFO, NOTIF_TARGET_USER_PREF, 6);
            tftMgr.setPage(7); // Jump to Network Monitor page
        }
    }

    void sendReply(const String& msg) {
        if (deviceConnected && pTxCharacteristic) {
            pTxCharacteristic->setValue(msg.c_str());
            pTxCharacteristic->notify();
        }
    }

    void onWrite(BLECharacteristic* pCharacteristic) override {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            String input = String(rxValue.c_str());
            input.trim();
            Serial.println("📡 BLE RX: " + input);

            if (input.equalsIgnoreCase("help") || input == "?") {
                sendReply("Send WiFi credentials as: SSID,PASSWORD or SSID\\nPASSWORD\r\n");
                return;
            }
            if (input.equalsIgnoreCase("status")) {
                String stat = "WIFI: " + String(WiFi.status() == WL_CONNECTED ? "ONLINE" : "OFFLINE") + 
                              " | IP: " + WiFi.localIP().toString() + "\r\n";
                sendReply(stat);
                return;
            }

            int sepIdx = input.indexOf('\n');
            if (sepIdx == -1) sepIdx = input.indexOf('\r');
            if (sepIdx == -1) sepIdx = input.indexOf(',');

            if (sepIdx > 0) {
                String newSsid = input.substring(0, sepIdx);
                String newPass = input.substring(sepIdx + 1);
                newSsid.trim();
                newPass.trim();

                if (!newSsid.isEmpty()) {
                    configMgr.config.wifiSsid = newSsid;
                    configMgr.config.wifiPass = newPass;
                    configMgr.saveConfig();

                    sendReply("✅ WiFi Saved: " + newSsid + "\r\nConnecting in 2 seconds...\r\n");
                    notificationMgr.trigger("WiFi Config RX", "Saved: " + newSsid, NOTIF_INFO, NOTIF_TARGET_USER_PREF, 4);
                    
                    pendingReconnect = true;
                    reconnectTriggerMs = millis();
                } else {
                    sendReply("❌ Invalid SSID. Format: SSID,PASSWORD\r\n");
                }
            } else {
                sendReply("❌ Format error! Please separate SSID and PASSWORD with a comma or newline.\r\n");
            }
        }
    }
};

#endif // BLE_WIFI_PROVISION_H
