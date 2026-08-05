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
    uint16_t enabledPagesMask = 0x3FFF; // All 14 pages enabled by default (bitmask 0b11111111111111)

    // To-Do List State Persistence
    String todoTitles[4] = { "Ship Desky", "Deep Pomo", "Hydration", "Telemetry" };
    bool todoChecked[4] = { true, false, false, false };

    int oledMode           = 0;  // 0: HUD, 1: Clock, 2: Sparklines, 3: Marquee Text, 4: Animated Screensavers, 5: WiFi Info
    int oledClockStyle     = 0;  // 0: Digital HUD, 1: Analog Minimal, 2: Cyber Matrix, 3: Retro Flip, 4: Vertical Stack, 5: Binary Segment, 6: Cyberpunk Box, 7: Radial Horizon
    uint8_t oledContrast   = 255;
    bool oledInverted      = false;
    String customText      = "Welcome to ChaosDesky!";

    int pomoWorkMins       = POMODORO_WORK_MINS;
    int pomoBreakMins      = POMODORO_BREAK_MINS;

    // Feature Toggles for CPU / Memory / Power Optimization
    bool featureBleEnabled        = false; // BLE WiFi Provisioning — DISABLED by default.
                                           // Conflicts with BLE HID Keyboard (shared BLE stack).
                                           // Use the web dashboard to configure WiFi instead.
    bool featureWeatherEnabled    = true;  // Enable OWM cloud HTTPS fetches
    bool featureScreensaverEnabled= true;  // Enable OLED Animated Screensaver engine

    // System Change & Alert Notifications Display Target (0=TFT Only, 1=OLED Only, 2=Both Displays)
    int notifTarget = 1; // Default to 1 (OLED only) instead of 2 (Both TFT & OLED)

    // Dual-Button Simultaneous Combo Action
    int btnCombo = 7;  // ACT_WIFI_INFO

    // HID Macro Configuration
    String hidMacroLeft = DEFAULT_MACRO_LEFT;
    String hidMacroRight = DEFAULT_MACRO_RIGHT;
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

    bool begin();
    bool loadConfig();
    bool saveConfig();
};

#endif // CONFIG_MANAGER_H
