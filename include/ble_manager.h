#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "config.h"

enum ProximityState {
    PROX_AWAY = 0,
    PROX_NEAR = 1,
    PROX_IMMEDIATE = 2
};

struct BleRadarData {
    bool enabled = true;
    bool autoWake = true;
    int rssiThreshold = -75; // Near threshold in dBm
    int currentRssi = -100;
    ProximityState state = PROX_AWAY;
    String targetDeviceMac = "";
    int targetDeviceCount = 0;
    unsigned long lastSeenMs = 0;
};

class BleRadarManager : public BLEAdvertisedDeviceCallbacks {
public:
    BleRadarData radar;

    bool begin() {
        BLEDevice::init("ChaosDesky-Radar");
        pBLEScan = BLEDevice::getScan();
        pBLEScan->setAdvertisedDeviceCallbacks(this);
        pBLEScan->setActiveScan(true); // Active scan for faster response
        pBLEScan->setInterval(100);
        pBLEScan->setWindow(99);
        Serial.println("✅ BLE Proximity Radar Initialized!");
        return true;
    }

    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        if (!radar.enabled) return;

        String address = advertisedDevice.getAddress().toString().c_str();
        int rssi = advertisedDevice.getRSSI();

        // Check for Apple Manufacturer Data (Apple Vendor ID: 0x004C) or specific MAC
        bool isAppleDevice = false;
        if (advertisedDevice.haveManufacturerData()) {
            std::string mData = advertisedDevice.getManufacturerData();
            if (mData.length() >= 2) {
                uint16_t vendorId = (uint8_t)mData[0] | ((uint8_t)mData[1] << 8);
                if (vendorId == 0x004C) { // Apple ID
                    isAppleDevice = true;
                }
            }
        }

        bool isTarget = isAppleDevice;
        if (!radar.targetDeviceMac.isEmpty()) {
            if (address.equalsIgnoreCase(radar.targetDeviceMac)) {
                isTarget = true;
            }
        }

        if (isTarget) {
            // Apply Exponential Moving Average filter to RSSI
            if (radar.currentRssi == -100) {
                radar.currentRssi = rssi;
            } else {
                radar.currentRssi = (int)(0.6f * radar.currentRssi + 0.4f * rssi);
            }

            radar.lastSeenMs = millis();

            // Evaluate Proximity State
            ProximityState newState = PROX_AWAY;
            if (radar.currentRssi >= -60) {
                newState = PROX_IMMEDIATE; // Very close (< 1 meter)
            } else if (radar.currentRssi >= radar.rssiThreshold) {
                newState = PROX_NEAR; // Near desk (~1.5 meters)
            } else {
                newState = PROX_AWAY;
            }

            if (newState != radar.state) {
                radar.state = newState;
                Serial.printf("📡 BLE Radar Proximity State Changed: %d (RSSI: %d dBm, Device: %s)\n", 
                              radar.state, radar.currentRssi, address.c_str());
            }
        }
    }

    void update() {
        if (!radar.enabled) return;

        unsigned long now = millis();
        // Periodically run async scan
        if (now - lastScanMs >= 4000) {
            lastScanMs = now;
            pBLEScan->start(2, false); // Scan for 2 seconds
            pBLEScan->clearResults();
        }

        // Check timeout for away state
        if (now - radar.lastSeenMs > 10000 && radar.state != PROX_AWAY) {
            radar.state = PROX_AWAY;
            radar.currentRssi = -100;
            Serial.println("📡 BLE Radar: iPhone Away / Timeout (Displays Dimming)");
        }
    }

private:
    BLEScan* pBLEScan = nullptr;
    unsigned long lastScanMs = 0;
};

#endif // BLE_MANAGER_H
