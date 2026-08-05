#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <LittleFS.h>

#include "config.h"
#include "config_manager.h"
#include "sensors.h"
#include "zambretti.h"
#include "weather_api.h"
#include "pomodoro.h"
#include "display_oled.h"
#include "display_tft.h"
#include "notification_manager.h"
#include "mech_switch.h"
#include "ble_wifi_provision.h"
#include "web_server.h"

// System Instances
ConfigManager          configMgr;
SensorManager          sensorMgr;
WeatherApiClient       weatherMgr;
PomodoroTimer          pomoTimer;
OledDisplayManager     oledMgr;
TftDisplayManager      tftMgr;
NotificationManager    notificationMgr;
MechSwitchManager      mechSwitch1Mgr; // D25: Page Navigation & Slideshow Toggle
MechSwitchManager      mechSwitch2Mgr; // D26: TFT Interactive Control & Actions
BleWifiProvisioner     bleWifiMgr;
ScreensaverEngine      screensaverEngine;
WatchFaceEngine        watchFaceEngine;
WebServerManager       webServerMgr(80);

// Timing Variables
unsigned long lastSensorReadMs    = 0;
unsigned long lastWeatherFetchMs   = 0;
unsigned long lastCarouselMs       = 0;
unsigned long lastOledRefreshMs    = 0;
unsigned long lastPressureSampleMs = 0;

String localIpStr = "0.0.0.0";
String timeStr    = "00:00:00";
unsigned long carouselIntervalMs = CAROUSEL_INTERVAL_MS;

// Helper: Format NTP time
String getFormattedNtpTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "00:00:00";
    }
    char timeBuf[12];
    strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &timeinfo);
    return String(timeBuf);
}

void setupWiFi() {
    String ssid = configMgr.config.wifiSsid;
    String pass = configMgr.config.wifiPass;

    if (ssid.isEmpty() || ssid == "YOUR_WIFI_SSID") {
        ssid = DEFAULT_WIFI_SSID;
        pass = DEFAULT_WIFI_PASS;
    }

    Serial.println("🌐 Connecting to WiFi: " + ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long startMs = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startMs < 10000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        localIpStr = WiFi.localIP().toString();
        Serial.println("\n✅ WiFi Connected! IP: " + localIpStr);
        configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER_1, NTP_SERVER_2);
    } else {
        Serial.println("\n⚠️ WiFi Connection Failed! Launching AP Mode...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(AP_SSID, AP_PASS);
        localIpStr = WiFi.softAPIP().toString();
        Serial.println("📡 AP Mode IP: " + localIpStr);
    }
}


DualSwitchComboDetector comboDetector;

void executeButtonAction(int action) {
    switch (action) {
        case ACT_CYCLE_OLED:
            configMgr.config.oledMode = (configMgr.config.oledMode + 1) % 7;
            oledMgr.oledMode = configMgr.config.oledMode;
            configMgr.saveConfig();
            Serial.printf("🔘 Macro Action: Cycled OLED Mode to %d\n", configMgr.config.oledMode);
            break;
            
        case ACT_NEXT_TFT_PAGE:
            tftMgr.nextPage();
            lastCarouselMs = millis();
            Serial.printf("🔘 Macro Action: Switched to TFT Page %d\n", tftMgr.currentPage);
            break;
            
        case ACT_TOGGLE_POMO:
            tftMgr.setPage(2); // Jump to Pomodoro page
            if (pomoTimer.state == POMO_IDLE || pomoTimer.state == POMO_PAUSED) {
                pomoTimer.startWork();
                notificationMgr.trigger("Pomodoro Timer", "Work Timer Started!", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 2);
            } else {
                pomoTimer.pause();
                notificationMgr.trigger("Pomodoro Timer", "Timer Paused", NOTIF_WARNING, NOTIF_TARGET_USER_PREF, 2);
            }
            break;
            
        case ACT_JUMP_TODO:
            if (tftMgr.currentPage == 4) {
                tftMgr.setPage(5); // Toggle back and forth with Climate P6
            } else {
                tftMgr.setPage(4); // Snap straight to To-Do Board P5
            }
            Serial.println("🔘 Macro Action: Jumped to To-Do Board!");
            break;
            
        case ACT_JUMP_WATCH:
            configMgr.config.oledMode = 1; // Big Clock Mode
            oledMgr.oledMode = 1;
            configMgr.config.oledClockStyle = (configMgr.config.oledClockStyle + 1) % 14;
            configMgr.saveConfig();
            if (tftMgr.currentPage != 6) {
                tftMgr.setPage(6); // Jump straight to Watchface Studio (Page 7, index 6)
            } else {
                watchFaceEngine.activeStyle = (watchFaceEngine.activeStyle + 1) % 15;
                tftMgr.forceRedraw();
            }
            Serial.println("🔘 Macro Action: Jumped & Cycled Watchface Studio!");
            break;
            
        case ACT_CYCLE_THEMES:
            configMgr.config.tftTheme = (configMgr.config.tftTheme + 1) % TOTAL_THEMES;
            configMgr.saveConfig();
            tftMgr.forceRedraw();
            notificationMgr.trigger("TFT Color Theme", "Theme Cycled!", NOTIF_INFO, NOTIF_TARGET_OLED, 2);
            break;
            
        case ACT_WIFI_INFO:
            configMgr.config.oledMode = 5; // Mode 5: Dedicated WiFi Credentials & IP Broadcast
            oledMgr.oledMode = 5;
            tftMgr.setPage(3);             // Page 4: System QR Code & IP Dashboard
            notificationMgr.trigger("WiFi Broadcast", "Broadcasting Credentials", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 3);
            Serial.println("🔘 Macro Action: Broadcasting WiFi & QR Code across both screens!");
            break;
            
        case ACT_SCREENSAVER:
            if (configMgr.config.oledMode == 4) {
                screensaverEngine.oledSaverMode = (screensaverEngine.oledSaverMode + 1) % 6;
                Serial.println("🔘 Macro Action: Cycled OLED Screensaver style");
                notificationMgr.trigger("Screensaver", "Style Cycled", NOTIF_INFO, NOTIF_TARGET_TFT, 2);
            } else {
                configMgr.config.oledMode = 4; // OLED animated screensavers
                oledMgr.oledMode = 4;
                Serial.println("🔘 Macro Action: Launched OLED Screensaver");
                notificationMgr.trigger("Screensaver", "Activated", NOTIF_INFO, NOTIF_TARGET_TFT, 2);
            }
            break;
            
        case ACT_RESET_POMO:
            pomoTimer.reset();
            tftMgr.setPage(2);
            notificationMgr.trigger("Pomodoro Timer", "Timer Reset to 25m!", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 2);
            break;
            
        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n==========================================");
    Serial.println("🚀 CHAOS DESKY - ESP32 DUAL DISPLAY HUB v1.0");
    Serial.println("==========================================");

    // Initialize LittleFS
    if (!LittleFS.begin(true)) {
        Serial.println("❌ LittleFS Filesystem Mount Failed!");
    } else {
        Serial.println("✅ LittleFS Mounted Successfully!");
    }

    // Load Persistent Config
    configMgr.begin();
    notificationMgr.userPreference = NOTIF_TARGET_OLED; // Forced to OLED only based on user request

    // Apply Loaded Configuration to Modules
    tftMgr.setRotation(configMgr.config.tftRotation);
    tftMgr.applyTheme((TFTTheme)configMgr.config.tftTheme);
    tftMgr.enabledPagesMask = configMgr.config.enabledPagesMask;
    for (int i = 0; i < 4; i++) {
        tftMgr.todoTitles[i] = configMgr.config.todoTitles[i];
        tftMgr.todoChecked[i] = configMgr.config.todoChecked[i];
    }
    carouselIntervalMs = configMgr.config.carouselSpeedSec * 1000;

    oledMgr.oledMode = configMgr.config.oledMode;
    oledMgr.setContrast(configMgr.config.oledContrast);
    oledMgr.setInverted(configMgr.config.oledInverted);
    oledMgr.customText = configMgr.config.customText;
    tftMgr.customBannerText = configMgr.config.customText;

    pomoTimer.workDurationMins = configMgr.config.pomoWorkMins;
    pomoTimer.breakDurationMins = configMgr.config.pomoBreakMins;

    // Initialize Sensors (must be before displays to ensure I2C is up)
    sensorMgr.begin();

    // Initialize Displays
    oledMgr.begin();
    tftMgr.begin();

    // Initialize Mechanical Switches
    mechSwitch1Mgr.begin(MECH_SWITCH_1_PIN);
    mechSwitch1Mgr.instantTrigger = true; // 🔥 Left Button triggers INSTANTLY on press (no 350ms delay)
    mechSwitch2Mgr.begin(MECH_SWITCH_2_PIN);

    // Initialize BLE keyboard removed — using wired Serial HID bridge instead.
    // Run bleWifiMgr only if feature is enabled (off by default).
    if (configMgr.config.featureBleEnabled) {
        bleWifiMgr.begin();
    }

    // Setup Network & Web Server
    setupWiFi();
    webServerMgr.begin(&sensorMgr, &weatherMgr, &pomoTimer, &tftMgr, &notificationMgr);

    // Fetch initial Outdoor Weather
    weatherMgr.fetchWeather(configMgr.config.openWeatherKey, 
                            configMgr.config.openWeatherCity, 
                            configMgr.config.openWeatherCountry);

    // Add initial historical sample
    sensorMgr.addHistorySample(sensorMgr.data.pressureHpa, sensorMgr.data.tempC, sensorMgr.data.humidity);

    Serial.println("🎉 System Initialization Complete!");
}

void executePageRightButtonAction(int page, SwitchAction action) {
    if (action == SWITCH_SINGLE_CLICK) {
        switch (page) {
            case 0: // Weather Page
                Serial.println("➡️ Right Button [Weather]: Refreshing weather data...");
                notificationMgr.trigger("Weather API", "Fetching Latest Weather...", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 2);
                weatherMgr.fetchWeather(configMgr.config.openWeatherKey, 
                                        configMgr.config.openWeatherCity, 
                                        configMgr.config.openWeatherCountry);
                tftMgr.forceRedraw();
                break;

            case 1: // Pressure Graph Page
            case 10: // Temp Graph Page
            case 11: // Humidity Graph Page
                Serial.println("➡️ Right Button: Graph pages have no secondary action.");
                break;

            case 2: // Pomodoro Page
                Serial.println("➡️ Right Button [Pomodoro]: Toggling Timer State");
                executeButtonAction(ACT_TOGGLE_POMO);
                tftMgr.forceRedraw();
                break;

            case 3: // System QR / Info Page
                Serial.println("➡️ Right Button [System]: Cycling OLED Display Mode");
                executeButtonAction(ACT_CYCLE_OLED);
                break;

            case 4: // To-Do List Page
                Serial.println("➡️ Right Button [To-Do]: Toggling task checkbox");
                tftMgr.todoChecked[tftMgr.todoSelectedIdx] = !tftMgr.todoChecked[tftMgr.todoSelectedIdx];
                configMgr.config.todoChecked[tftMgr.todoSelectedIdx] = tftMgr.todoChecked[tftMgr.todoSelectedIdx];
                configMgr.saveConfig();
                tftMgr.forceRedraw();
                {
                    String msg = tftMgr.todoChecked[tftMgr.todoSelectedIdx] ? "Task Completed!" : "Task Re-Opened!";
                    notificationMgr.trigger("To-Do Target", msg, NOTIF_INFO, NOTIF_TARGET_USER_PREF, 2);
                }
                break;

            case 5: // Indoor Climate Page
                Serial.println("➡️ Right Button [Climate]: Cycling Climate Theme");
                tftMgr.activeClimateTheme = (tftMgr.activeClimateTheme + 1) % 7;
                sensorMgr.readSensors(); // Keep the sensor read too just in case
                notificationMgr.trigger("Climate", "Theme Cycled", NOTIF_INFO, NOTIF_TARGET_OLED, 2);
                tftMgr.forceRedraw();
                break;

            case 6: // Watchface Studio Page (Index 6)
                Serial.println("➡️ Right Button [Watchface]: Cycling Watchface Style");
                watchFaceEngine.activeStyle = (watchFaceEngine.activeStyle + 1) % 15;
                notificationMgr.trigger("Watchface", "Style Cycled", NOTIF_INFO, NOTIF_TARGET_OLED, 2);
                tftMgr.forceRedraw();
                break;

            case 7: // Network Monitor Page (Index 7)
                Serial.println("➡️ Right Button [Network]: Broadcasting WiFi Info");
                executeButtonAction(ACT_WIFI_INFO);
                break;

            case 8: // System Hardware Stats Page (Index 8)
                Serial.println("➡️ Right Button [Hardware]: Cycling TFT Theme");
                executeButtonAction(ACT_CYCLE_THEMES);
                break;

            case 9: // OLED Display Hub Page (Index 9)
                Serial.println("➡️ Right Button [OLED Hub]: Cycling OLED Display Mode");
                executeButtonAction(ACT_CYCLE_OLED);
                tftMgr.forceRedraw();
                {
                    const char* modeNames[] = {"Telemetry HUD", "Dynamic Clock", "Sparklines", "Marquee Ticker", "Screensavers", "WiFi Specs", "Indoor Climate"};
                    notificationMgr.trigger("OLED Mode", modeNames[configMgr.config.oledMode], NOTIF_INFO, NOTIF_TARGET_TFT, 2);
                }
                break;

            default:
                break;
        }
    } else if (action == SWITCH_DOUBLE_CLICK) {
        switch (page) {
            case 2: // Pomodoro: Reset timer
                Serial.println("➡️ Right Button Double-Click [Pomodoro]: Resetting timer");
                executeButtonAction(ACT_RESET_POMO);
                break;

            case 4: // To-Do List: Move focus item
                tftMgr.todoSelectedIdx = (tftMgr.todoSelectedIdx + 1) % 4;
                tftMgr.forceRedraw();
                Serial.printf("➡️ Right Button Double-Click [To-Do]: Moved focus to item %d\n", tftMgr.todoSelectedIdx + 1);
                break;

            case 6: // Watchface: Switch to Retro Flip (Index 4)
                watchFaceEngine.activeStyle = WATCHFACE_RETRO_FLIP;
                tftMgr.forceRedraw();
                notificationMgr.trigger("Retro Flip", "Vintage Clock Active!", NOTIF_INFO, NOTIF_TARGET_OLED, 2);
                break;

            case 9: // OLED Display Hub: Cycle Clock Style
                Serial.println("➡️ Right Button Double-Click [OLED Hub]: Cycling OLED Clock Style");
                configMgr.config.oledClockStyle = (configMgr.config.oledClockStyle + 1) % 14;
                configMgr.saveConfig();
                tftMgr.forceRedraw();
                notificationMgr.trigger("OLED Studio", "Clock Style Cycled!", NOTIF_INFO, NOTIF_TARGET_TFT, 2);
                break;

            default:
                executeButtonAction(ACT_JUMP_TODO);
                break;
        }
    } else if (action == SWITCH_LONG_PRESS) {
        switch (page) {
            case 2: // Pomodoro: Reset timer
                Serial.println("➡️ Right Button Long-Hold [Pomodoro]: Resetting timer");
                executeButtonAction(ACT_RESET_POMO);
                break;
            default:
                Serial.println("➡️ Right Button Long-Hold [Global]: Toggling Screensavers");
                executeButtonAction(ACT_SCREENSAVER);
                break;
        }
    }
}

void loop() {
    unsigned long currentMs = millis();

    // 1. Update State Machines & Dual Mechanical Switches
    pomoTimer.update();
    notificationMgr.update();
    if (configMgr.config.featureBleEnabled) {
        bleWifiMgr.update();
    }

    // ==============================================================
    // 🤝 SIMULTANEOUS COMBO CHECK (LEFT + RIGHT HELD TOGETHER)
    // ==============================================================
    int comboState = comboDetector.update(mechSwitch1Mgr, mechSwitch2Mgr);
    if (comboState == 1) {
        Serial.println("🤝 Simultaneous Dual-Button Combo Detected!");
        executeButtonAction(configMgr.config.btnCombo);
    } else if (comboState == 2) {
        // Long combo: cycle OLED modes
        configMgr.config.oledMode = (configMgr.config.oledMode + 1) % 7;
        oledMgr.oledMode = configMgr.config.oledMode;
        configMgr.saveConfig();
        notificationMgr.trigger("OLED Mode", "Mode " + String(configMgr.config.oledMode), NOTIF_INFO, NOTIF_TARGET_OLED, 2);
    }

    // ==============================================================
    // ⬅️ LEFT KEY (D25) — CYCLE THROUGH PAGES
    // ==============================================================
    SwitchAction action1 = mechSwitch1Mgr.update();
    if (action1 == SWITCH_SINGLE_CLICK) {
        tftMgr.nextPage();
        lastCarouselMs = millis();
        Serial.printf("⬅️ Left Button Single-Click: Cycled to TFT Page %d\n", tftMgr.currentPage);
    } else if (action1 == SWITCH_DOUBLE_CLICK) {
        tftMgr.prevPage();
        lastCarouselMs = millis();
        Serial.printf("⬅️ Left Button Double-Click: Cycled back to TFT Page %d\n", tftMgr.currentPage);
    } else if (action1 == SWITCH_LONG_PRESS) {
        tftMgr.setPage(0);
        lastCarouselMs = millis();
        Serial.println("⬅️ Left Button Long-Press: Jumped to Page 0 (Home)");
    }

    // ==============================================================
    // ➡️ RIGHT KEY (D26) — PAGE-SPECIFIC FUNCTIONALITY
    // ==============================================================
    SwitchAction action2 = mechSwitch2Mgr.update();
    if (action2 != SWITCH_NO_ACTION) {
        executePageRightButtonAction(tftMgr.currentPage, action2);
    }

    // 2. Periodic Sensor Sampling (Every 2 seconds)
    if (currentMs - lastSensorReadMs >= 2000) {
        lastSensorReadMs = currentMs;
        sensorMgr.readSensors();
    }

    // 3. Periodic Pressure Sampling for Trend Sparkline (Every 15 mins)
    if (currentMs - lastPressureSampleMs >= PRESSURE_SAMPLE_MS) {
        lastPressureSampleMs = currentMs;
        // Log sensor sample to sparkline arrays
        sensorMgr.addHistorySample(sensorMgr.data.pressureHpa, sensorMgr.data.tempC, sensorMgr.data.humidity);
    }

    // 4. Periodic Weather API Fetch (Every 10 mins, if enabled)
    if (configMgr.config.featureWeatherEnabled && (currentMs - lastWeatherFetchMs >= WEATHER_UPDATE_MS)) {
        lastWeatherFetchMs = currentMs;
        weatherMgr.fetchWeather(configMgr.config.openWeatherKey, 
                                configMgr.config.openWeatherCity, 
                                configMgr.config.openWeatherCountry);
    }

    // 5. TFT Carousel Page Switcher
    if (carouselIntervalMs > 0 && !notificationMgr.isTftActive() && (currentMs - lastCarouselMs >= carouselIntervalMs)) {
        lastCarouselMs = currentMs;
        tftMgr.nextPage();
    }

    // 6. Update Displays (Dynamic Refresh for High-Speed Marquee Ticker)
    uint32_t oledInterval = 500;
    if (configMgr.config.oledMode == 3) {
        oledInterval = 45; // Fast scrolling speed for Custom Marquee Ticker
    } else if (configMgr.config.oledMode == 4 || notificationMgr.isOledActive()) {
        oledInterval = 75; // Smooth animations for OLED screensavers & notifications
    }
    if (currentMs - lastOledRefreshMs >= oledInterval) {
        lastOledRefreshMs = currentMs;
        struct tm timeinfo;
        int h = 0, m = 0, s_val = 0;
        if (getLocalTime(&timeinfo)) {
            h = timeinfo.tm_hour;
            m = timeinfo.tm_min;
            s_val = timeinfo.tm_sec;
        }
        int rssi = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : 0;
        
        oledMgr.draw(sensorMgr, notificationMgr, localIpStr, h, m, s_val, rssi, configMgr.config.oledClockStyle);
        tftMgr.renderCurrentPage(weatherMgr.weather, sensorMgr, pomoTimer, notificationMgr, h, m, s_val, localIpStr, configMgr.config.oledMode, configMgr.config.oledClockStyle);
    }
}
