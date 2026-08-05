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

    void begin();
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;
    void update();
    void sendReply(const String& msg);
    void onWrite(BLECharacteristic* pCharacteristic) override;
};

#endif // BLE_WIFI_PROVISION_H
