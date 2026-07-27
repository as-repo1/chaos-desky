#ifndef BLE_UART_SERVER_H
#define BLE_UART_SERVER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "display_tft.h"
#include "pomodoro.h"
#include "notification_manager.h"
#include "sensors.h"
#include "config_manager.h"

extern TftDisplayManager tftMgr;
extern WatchFaceEngine watchFaceEngine;
extern PomodoroTimer pomoTimer;
extern NotificationManager notificationMgr;
extern SensorManager sensorMgr;
extern ConfigManager configMgr;

// Nordic UART Service (NUS) Standard UUIDs
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class BleUartServer : public BLEServerCallbacks, public BLECharacteristicCallbacks {
public:
    BLEServer* pServer = nullptr;
    BLECharacteristic* pTxCharacteristic = nullptr;
    bool deviceConnected = false;
    bool oldDeviceConnected = false;
    String latestTelemetry = "No PC Data";

    void begin() {
        BLEDevice::init("chaos-desky");
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
        pAdvertising->setMinPreferred(0x06); // iPhone connection parameters
        pAdvertising->setMaxPreferred(0x12);
        BLEDevice::startAdvertising();

        Serial.println("📡 BLE Offline UART & PC Telemetry Server Started (Service: NUS)");
    }

    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("📲 Bluetooth Terminal / PC Telemetry Connected!");
        notificationMgr.trigger("BLE Connected", "UART Server & Telemetry Active", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 5);
        
        // Welcome Banner
        sendReply("\r\n===========================\r\n🚀 CHAOS DESKY BLE UART\r\nType 'help' for commands\r\n===========================\r\n");
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        Serial.println("📲 Bluetooth Disconnected. Re-advertising...");
    }

    void update() {
        if (!deviceConnected && oldDeviceConnected) {
            delay(500); // Give BT stack chance to clean up
            pServer->startAdvertising();
            oldDeviceConnected = deviceConnected;
        }
        if (deviceConnected && !oldDeviceConnected) {
            oldDeviceConnected = deviceConnected;
        }
    }

    void sendReply(const String& msg) {
        if (deviceConnected && pTxCharacteristic) {
            pTxCharacteristic->setValue(msg.c_str());
            pTxCharacteristic->notify();
        }
    }

    // Handle Incoming Command or Telemetry from Phone / PC
    void onWrite(BLECharacteristic* pCharacteristic) override {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            String cmd = String(rxValue.c_str());
            cmd.trim();
            Serial.println("📡 BLE UART RX: " + cmd);

            if (cmd.equalsIgnoreCase("help") || cmd == "?") {
                sendReply("COMMANDS:\r\n"
                          " casio    -> Iconic F-91W Watchface\r\n"
                          " gshock   -> Casio G-Shock Watchface\r\n"
                          " page <N> -> Switch TFT Page 0..9\r\n"
                          " pomo <start|pause|reset>\r\n"
                          " status   -> Read Sensors & Network\r\n"
                          " pc:<msg> -> Broadcast PC Telemetry\r\n");
            } 
            else if (cmd.equalsIgnoreCase("status")) {
                char buf[128];
                snprintf(buf, sizeof(buf), "TEMP: %.1fC | HUM: %.0f%% | PRESS: %.1fhPa | HEAP: %u KB\r\n",
                         sensorMgr.data.tempC, sensorMgr.data.humidity, sensorMgr.data.pressureHpa, ESP.getFreeHeap()/1024);
                sendReply(String(buf));
            }
            else if (cmd.equalsIgnoreCase("casio") || cmd.equalsIgnoreCase("f91w")) {
                watchFaceEngine.activeStyle = WATCHFACE_CASIO_F91W;
                tftMgr.setPage(7);
                sendReply("✅ Switched to Casio F-91W Watchface\r\n");
            }
            else if (cmd.equalsIgnoreCase("gshock")) {
                watchFaceEngine.activeStyle = WATCHFACE_CASIO_GSHOCK;
                tftMgr.setPage(7);
                sendReply("✅ Switched to Casio G-Shock Watchface\r\n");
            }
            else if (cmd.startsWith("page ") || cmd.startsWith("PAGE ")) {
                int pg = cmd.substring(5).toInt();
                tftMgr.setPage(pg);
                sendReply("✅ TFT Page set to " + String(pg) + "\r\n");
            }
            else if (cmd.equalsIgnoreCase("pomo start")) {
                pomoTimer.startWork();
                tftMgr.setPage(2);
                sendReply("⏳ Pomodoro Work Started!\r\n");
            }
            else if (cmd.equalsIgnoreCase("pomo pause")) {
                pomoTimer.pause();
                sendReply("⏸️ Pomodoro Paused\r\n");
            }
            else if (cmd.equalsIgnoreCase("pomo reset")) {
                pomoTimer.reset();
                sendReply("🔄 Pomodoro Reset\r\n");
            }
            else if (cmd.startsWith("pc:") || cmd.startsWith("PC:") || cmd.startsWith("msg:")) {
                String text = cmd.substring(3);
                latestTelemetry = text;
                notificationMgr.trigger("PC Telemetry", text, NOTIF_INFO, NOTIF_TARGET_USER_PREF, 10);
                sendReply("✅ Telemetry Displayed on Hub\r\n");
            }
            else {
                sendReply("❌ Unknown command. Type 'help' for options.\r\n");
            }
        }
    }
};

#endif // BLE_UART_SERVER_H
