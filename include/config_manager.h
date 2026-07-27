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
    int carouselSpeedSec   = 0;  // 0 = pause (Manual Navigation Only)
    uint16_t enabledPagesMask = 0x03FF; // All 10 pages enabled by default (bitmask 0b1111111111)

    int oledMode           = 0;  // 0: HUD, 1: Clock, 2: Sparklines, 3: Marquee Text, 4: Custom Bitmap
    int oledClockStyle     = 0;  // 0: Digital HUD, 1: Analog Minimal, 2: Cyber Matrix, 3: Retro Flip, 4: Vertical Stack, 5: Binary Segment
    uint8_t oledContrast   = 255;
    bool oledInverted      = false;
    String customText      = "Welcome to ChaosDesky!";

    int pomoWorkMins       = POMODORO_WORK_MINS;
    int pomoBreakMins      = POMODORO_BREAK_MINS;

    bool ancsEnabled       = true;
    bool ancsAutoPopup     = true;

    // Feature Toggles for CPU / Memory / Power Optimization
    bool featureBleEnabled        = true; // Enable BLE UART radio
    bool featureWeatherEnabled    = true; // Enable OWM cloud HTTPS fetches
    bool featureScreensaverEnabled= true; // Enable TFT Warp Screensaver math

    // System Change & Alert Notifications Display Target (0=TFT Only, 1=OLED Only, 2=Both Displays)
    int notifTarget = 2;

    // Custom Macro & Button Studio Mappings
    int btnLeftSingle   = 2;  // ACT_NEXT_TFT_PAGE (Left single click switches TFT pages)
    int btnLeftDouble   = 4;  // ACT_JUMP_TODO (Left double click enters To-Do functionality)
    int btnLeftLong     = 3;  // ACT_TOGGLE_POMO
    int btnRightSingle  = 1;  // ACT_CYCLE_OLED (Right single click switches OLED modes)
    int btnRightDouble  = 5;  // ACT_JUMP_WATCH (Right double click enters Watch Studio functionality)
    int btnRightLong    = 6;  // ACT_CYCLE_THEMES
    int btnCombo        = 7;  // ACT_WIFI_INFO
};

enum CustomButtonAction {
    ACT_DEFAULT_NAV     = 0,
    ACT_CYCLE_OLED      = 1,
    ACT_NEXT_TFT_PAGE   = 2,
    ACT_TOGGLE_POMO     = 3,
    ACT_JUMP_TODO       = 4,
    ACT_JUMP_WATCH      = 5,
    ACT_CYCLE_THEMES    = 6,
    ACT_WIFI_INFO       = 7,
    ACT_SCREENSAVER     = 8,
    ACT_RESET_POMO      = 9,
    ACT_NO_ACTION       = 10
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

        StaticJsonDocument<1536> doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            Serial.println("❌ Failed to parse /config.json: " + String(error.c_str()));
            return false;
        }

        String s = doc["wifiSsid"] | DEFAULT_WIFI_SSID;
        String p = doc["wifiPass"] | DEFAULT_WIFI_PASS;

        if (s.isEmpty() || s == "YOUR_WIFI_SSID") {
            s = DEFAULT_WIFI_SSID;
            p = DEFAULT_WIFI_PASS;
        }

        config.wifiSsid        = s;
        config.wifiPass        = p;
        config.openWeatherKey  = doc["openWeatherKey"] | OPENWEATHER_API_KEY;
        config.openWeatherCity = doc["openWeatherCity"] | OPENWEATHER_CITY;
        config.openWeatherCountry = doc["openWeatherCountry"] | OPENWEATHER_COUNTRY;

        config.tftTheme        = doc["tftTheme"] | 0;
        config.tftRotation     = doc["tftRotation"] | 2;
        config.carouselSpeedSec= doc["carouselSpeedSec"] | 0;
        config.enabledPagesMask= doc["enabledPagesMask"] | 0x03FF;

        config.oledMode        = doc["oledMode"] | 0;
        config.oledClockStyle  = doc["oledClockStyle"] | 0;
        config.oledContrast    = doc["oledContrast"] | 255;
        config.oledInverted    = doc["oledInverted"] | false;
        config.customText      = doc["customText"] | "Welcome to ChaosDesky!";

        config.pomoWorkMins    = doc["pomoWorkMins"] | POMODORO_WORK_MINS;
        config.pomoBreakMins   = doc["pomoBreakMins"] | POMODORO_BREAK_MINS;

        config.ancsEnabled     = doc["ancsEnabled"] | true;
        config.ancsAutoPopup   = doc["ancsAutoPopup"] | true;
        config.featureBleEnabled         = doc["featureBleEnabled"] | true;
        config.featureWeatherEnabled     = doc["featureWeatherEnabled"] | true;
        config.featureScreensaverEnabled = doc["featureScreensaverEnabled"] | true;
        config.notifTarget               = doc["notifTarget"] | 2;

        config.btnLeftSingle  = doc["btnLeftSingle"]  | 2;
        config.btnLeftDouble  = doc["btnLeftDouble"]  | 4;
        config.btnLeftLong    = doc["btnLeftLong"]    | 3;
        config.btnRightSingle = doc["btnRightSingle"] | 1;
        config.btnRightDouble = doc["btnRightDouble"] | 5;
        config.btnRightLong   = doc["btnRightLong"]   | 6;
        config.btnCombo       = doc["btnCombo"]       | 7;

        Serial.println("✅ Configuration loaded successfully from LittleFS!");
        return true;
    }

    bool saveConfig() {
        File file = LittleFS.open("/config.json", "w");
        if (!file) {
            Serial.println("❌ Failed to open /config.json for writing!");
            return false;
        }

        StaticJsonDocument<1536> doc;
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
        doc["oledClockStyle"]    = config.oledClockStyle;
        doc["oledContrast"]     = config.oledContrast;
        doc["oledInverted"]     = config.oledInverted;
        doc["customText"]       = config.customText;

        doc["pomoWorkMins"]     = config.pomoWorkMins;
        doc["pomoBreakMins"]    = config.pomoBreakMins;

        doc["ancsEnabled"]              = config.ancsEnabled;
        doc["ancsAutoPopup"]            = config.ancsAutoPopup;
        doc["featureBleEnabled"]        = config.featureBleEnabled;
        doc["featureWeatherEnabled"]    = config.featureWeatherEnabled;
        doc["featureScreensaverEnabled"]= config.featureScreensaverEnabled;
        doc["notifTarget"]              = config.notifTarget;

        doc["btnLeftSingle"]  = config.btnLeftSingle;
        doc["btnLeftDouble"]  = config.btnLeftDouble;
        doc["btnLeftLong"]    = config.btnLeftLong;
        doc["btnRightSingle"] = config.btnRightSingle;
        doc["btnRightDouble"] = config.btnRightDouble;
        doc["btnRightLong"]   = config.btnRightLong;
        doc["btnCombo"]       = config.btnCombo;

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
