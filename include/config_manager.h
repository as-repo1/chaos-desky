#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"

struct SystemConfig {
    String wifiSsid        = DEFAULT_WIFI_SSID;
    String wifiPass        = DEFAULT_WIFI_PASS;
    String openWeatherKey  = OPENWEATHER_API_KEY;
    String openWeatherCity = OPENWEATHER_CITY;
    String openWeatherCountry = OPENWEATHER_COUNTRY;

    int tftTheme           = 0;  // 0-10 themes
    int tftRotation        = 2;  // 0-3
    int carouselSpeedSec   = 10; // 0 = pause
    uint16_t enabledPagesMask = 0x03FF; // All 10 pages enabled by default (bitmask 0b1111111111)

    int oledMode           = 0;  // 0: HUD, 1: Clock, 2: Sparklines, 3: Marquee Text, 4: Custom Bitmap
    uint8_t oledContrast   = 255;
    bool oledInverted      = false;
    String customText      = "Welcome to ChaosDesky!";

    int pomoWorkMins       = POMODORO_WORK_MINS;
    int pomoBreakMins      = POMODORO_BREAK_MINS;

    bool ancsEnabled       = true;
    bool ancsAutoPopup     = true;
};

class ConfigManager {
public:
    SystemConfig config;

    bool begin() {
        if (!LittleFS.exists("/config.json")) {
            Serial.println("⚙️ No /config.json found. Saving default configuration...");
            return saveConfig();
        }
        return loadConfig();
    }

    bool loadConfig() {
        File file = LittleFS.open("/config.json", "r");
        if (!file) {
            Serial.println("❌ Failed to open /config.json for reading!");
            return false;
        }

        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            Serial.println("❌ Failed to parse /config.json: " + String(error.c_str()));
            return false;
        }

        config.wifiSsid        = doc["wifiSsid"] | DEFAULT_WIFI_SSID;
        config.wifiPass        = doc["wifiPass"] | DEFAULT_WIFI_PASS;
        config.openWeatherKey  = doc["openWeatherKey"] | OPENWEATHER_API_KEY;
        config.openWeatherCity = doc["openWeatherCity"] | OPENWEATHER_CITY;
        config.openWeatherCountry = doc["openWeatherCountry"] | OPENWEATHER_COUNTRY;

        config.tftTheme        = doc["tftTheme"] | 0;
        config.tftRotation     = doc["tftRotation"] | 2;
        config.carouselSpeedSec= doc["carouselSpeedSec"] | 10;
        config.enabledPagesMask= doc["enabledPagesMask"] | 0x03FF;

        config.oledMode        = doc["oledMode"] | 0;
        config.oledContrast    = doc["oledContrast"] | 255;
        config.oledInverted    = doc["oledInverted"] | false;
        config.customText      = doc["customText"] | "Welcome to ChaosDesky!";

        config.pomoWorkMins    = doc["pomoWorkMins"] | POMODORO_WORK_MINS;
        config.pomoBreakMins   = doc["pomoBreakMins"] | POMODORO_BREAK_MINS;

        config.ancsEnabled     = doc["ancsEnabled"] | true;
        config.ancsAutoPopup   = doc["ancsAutoPopup"] | true;

        Serial.println("✅ Configuration loaded successfully from LittleFS!");
        return true;
    }

    bool saveConfig() {
        File file = LittleFS.open("/config.json", "w");
        if (!file) {
            Serial.println("❌ Failed to open /config.json for writing!");
            return false;
        }

        StaticJsonDocument<1024> doc;
        doc["wifiSsid"]         = config.wifiSsid;
        doc["wifiPass"]         = config.wifiPass;
        doc["openWeatherKey"]   = config.openWeatherKey;
        doc["openWeatherCity"]  = config.openWeatherCity;
        doc["openWeatherCountry"]= config.openWeatherCountry;

        doc["tftTheme"]         = config.tftTheme;
        doc["tftRotation"]      = config.tftRotation;
        doc["carouselSpeedSec"] = config.carouselSpeedSec;
        doc["enabledPagesMask"] = config.enabledPagesMask;

        doc["oledMode"]         = config.oledMode;
        doc["oledContrast"]     = config.oledContrast;
        doc["oledInverted"]     = config.oledInverted;
        doc["customText"]       = config.customText;

        doc["pomoWorkMins"]     = config.pomoWorkMins;
        doc["pomoBreakMins"]    = config.pomoBreakMins;

        doc["ancsEnabled"]      = config.ancsEnabled;
        doc["ancsAutoPopup"]    = config.ancsAutoPopup;

        if (serializeJson(doc, file) == 0) {
            Serial.println("❌ Failed to write JSON to /config.json!");
            file.close();
            return false;
        }

        file.close();
        Serial.println("✅ Saved configuration to LittleFS /config.json!");
        return true;
    }
};

#endif // CONFIG_MANAGER_H
