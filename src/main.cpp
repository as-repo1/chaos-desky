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
#include "ancs_client.h"
#include "mech_switch.h"
#include "ble_uart_server.h"
#include "web_server.h"

// System Instances
ConfigManager          configMgr;
SensorManager          sensorMgr;
WeatherApiClient       weatherMgr;
PomodoroTimer          pomoTimer;
OledDisplayManager     oledMgr;
TftDisplayManager      tftMgr;
NotificationManager    notificationMgr;
AncsNotificationClient ancsClientMgr;
MechSwitchManager      mechSwitch1Mgr; // D25: Page Navigation & Slideshow Toggle
MechSwitchManager      mechSwitch2Mgr; // D26: TFT Interactive Control & Actions
BleUartServer          bleUartMgr;
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
            configMgr.config.oledMode = (configMgr.config.oledMode + 1) % 5;
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
                tftMgr.setPage(6); // Toggle back and forth with Notes / Notif log
            } else {
                tftMgr.setPage(4); // Snap straight to To-Do Board
            }
            Serial.println("🔘 Macro Action: Jumped to To-Do & Notes Board!");
            break;
            
        case ACT_JUMP_WATCH:
            configMgr.config.oledMode = 1; // Big Clock Mode
            configMgr.config.oledClockStyle = (configMgr.config.oledClockStyle + 1) % 8;
            configMgr.saveConfig();
            if (tftMgr.currentPage != 7) {
                tftMgr.setPage(7); // Jump straight to Watchface Studio
            } else {
                watchFaceEngine.activeStyle = (watchFaceEngine.activeStyle + 1) % 10;
                tftMgr.forceRedraw();
            }
            Serial.println("🔘 Macro Action: Jumped & Cycled Watchface Studio!");
            break;
            
        case ACT_CYCLE_THEMES:
            if (tftMgr.currentPage == 7) {
                watchFaceEngine.activeStyle = WATCHFACE_CASIO_F91W;
                tftMgr.forceRedraw();
                notificationMgr.trigger("Casio F-91W", "Iconic Watchface Active!", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 2);
            } else {
                configMgr.config.tftTheme = (configMgr.config.tftTheme + 1) % TOTAL_THEMES;
                configMgr.saveConfig();
                tftMgr.forceRedraw();
                notificationMgr.trigger("TFT Color Theme", "Theme Cycled!", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 2);
            }
            break;
            
        case ACT_WIFI_INFO:
            configMgr.config.oledMode = 6; // Mode 6: Dedicated WiFi Credentials & IP Broadcast
            tftMgr.setPage(3);             // Page 3: System QR Code & IP Dashboard
            notificationMgr.trigger("WiFi Broadcast", "Broadcasting Credentials", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 3);
            Serial.println("🔘 Macro Action: Broadcasting WiFi & QR Code across both screens!");
            break;
            
        case ACT_SCREENSAVER:
            configMgr.config.oledMode = 5; // OLED screensaver
            tftMgr.setPage(9);             // Warp screensaver page
            Serial.println("🔘 Macro Action: Launched Dual Screensavers!");
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
    notificationMgr.userPreference = (NotificationTarget)configMgr.config.notifTarget;

    // Apply Loaded Configuration to Modules
    tftMgr.setRotation(configMgr.config.tftRotation);
    tftMgr.applyTheme((TFTTheme)configMgr.config.tftTheme);
    tftMgr.enabledPagesMask = configMgr.config.enabledPagesMask;
    carouselIntervalMs = configMgr.config.carouselSpeedSec * 1000;

    oledMgr.oledMode = configMgr.config.oledMode;
    oledMgr.setContrast(configMgr.config.oledContrast);
    oledMgr.setInverted(configMgr.config.oledInverted);
    oledMgr.customText = configMgr.config.customText;
    tftMgr.customBannerText = configMgr.config.customText;

    pomoTimer.workDurationMins = configMgr.config.pomoWorkMins;
    pomoTimer.breakDurationMins = configMgr.config.pomoBreakMins;

    // Initialize Displays
    oledMgr.begin();
    tftMgr.begin();

    // Initialize Sensors, Dual Switches & BLE UART Receiver
    sensorMgr.begin();
    ancsClientMgr.begin();
    mechSwitch1Mgr.begin(MECH_SWITCH_1_PIN);
    mechSwitch2Mgr.begin(MECH_SWITCH_2_PIN);
    bleUartMgr.begin();

    // Setup Network & Web Server
    setupWiFi();
    webServerMgr.begin(&sensorMgr, &weatherMgr, &pomoTimer, &tftMgr, &ancsClientMgr, &notificationMgr);

    // Fetch initial Outdoor Weather
    weatherMgr.fetchWeather(configMgr.config.openWeatherKey, 
                            configMgr.config.openWeatherCity, 
                            configMgr.config.openWeatherCountry);

    // Add initial barometric pressure sample
    sensorMgr.addPressureSample(sensorMgr.data.pressureHpa);

    Serial.println("🎉 System Initialization Complete!");
}

void loop() {
    unsigned long currentMs = millis();

    // 1. Update State Machines & Dual Mechanical Switches
    pomoTimer.update();
    notificationMgr.update();
    if (configMgr.config.featureBleEnabled) {
        bleUartMgr.update();
    }

    // ==============================================================
    // 🤝 SIMULTANEOUS COMBO CHECK (LEFT + RIGHT HELD TOGETHER)
    // ==============================================================
    if (comboDetector.update(mechSwitch1Mgr, mechSwitch2Mgr)) {
        Serial.println("🤝 Simultaneous Dual-Button Combo Detected!");
        executeButtonAction(configMgr.config.btnCombo);
    }

    // ==============================================================
    // ⬅️ LEFT KEY (D25) — INTERACTIVE CONTEXT & MACRO STUDIO
    // ==============================================================
    SwitchAction action1 = mechSwitch1Mgr.update();
    if (action1 == SWITCH_SINGLE_CLICK) {
        // A1: Context-Aware Navigation on interactive screens!
        if (tftMgr.currentPage == 4) {
            // On To-Do List: move focus down through items
            tftMgr.todoSelectedIdx = (tftMgr.todoSelectedIdx + 1) % 4;
            tftMgr.forceRedraw();
            Serial.printf("⬅️ Context Control: Moved To-Do focus to item %d\n", tftMgr.todoSelectedIdx + 1);
        } else if (tftMgr.currentPage == 2) {
            // On Pomodoro: toggle work/pause
            executeButtonAction(ACT_TOGGLE_POMO);
        } else {
            // Execute Web-Configurable Single Click Macro
            executeButtonAction(configMgr.config.btnLeftSingle);
        }
    } else if (action1 == SWITCH_DOUBLE_CLICK) {
        executeButtonAction(configMgr.config.btnLeftDouble);
    } else if (action1 == SWITCH_LONG_PRESS) {
        executeButtonAction(configMgr.config.btnLeftLong);
    }

    // ==============================================================
    // ➡️ RIGHT KEY (D26) — INTERACTIVE ACTION & MACRO STUDIO
    // ==============================================================
    SwitchAction action2 = mechSwitch2Mgr.update();
    if (action2 == SWITCH_SINGLE_CLICK) {
        // A1: Context-Aware Action on interactive screens!
        if (tftMgr.currentPage == 4) {
            // On To-Do List: Toggle check mark of selected task!
            tftMgr.todoChecked[tftMgr.todoSelectedIdx] = !tftMgr.todoChecked[tftMgr.todoSelectedIdx];
            tftMgr.forceRedraw();
            String msg = tftMgr.todoChecked[tftMgr.todoSelectedIdx] ? "Task Completed!" : "Task Re-Opened!";
            notificationMgr.trigger("To-Do Target", msg, NOTIF_INFO, NOTIF_TARGET_USER_PREF, 2);
            Serial.println("➡️ Context Control: Toggled To-Do item checkbox!");
        } else if (tftMgr.currentPage == 2) {
            // On Pomodoro: Reset timer
            executeButtonAction(ACT_RESET_POMO);
        } else {
            // Execute Web-Configurable Single Click Macro
            executeButtonAction(configMgr.config.btnRightSingle);
        }
    } else if (action2 == SWITCH_DOUBLE_CLICK) {
        executeButtonAction(configMgr.config.btnRightDouble);
    } else if (action2 == SWITCH_LONG_PRESS) {
        executeButtonAction(configMgr.config.btnRightLong);
    }

    // 2. Periodic Sensor Sampling (Every 2 seconds)
    if (currentMs - lastSensorReadMs >= 2000) {
        lastSensorReadMs = currentMs;
        sensorMgr.readSensors();
    }

    // 3. Periodic Pressure Sampling for Trend Sparkline (Every 15 mins)
    if (currentMs - lastPressureSampleMs >= PRESSURE_SAMPLE_MS) {
        lastPressureSampleMs = currentMs;
        sensorMgr.addPressureSample(sensorMgr.data.pressureHpa);
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

    // 6. Update Displays (Dynamic 45ms Refresh for High-Speed Marquee Ticker)
    uint32_t oledInterval = 500;
    if (configMgr.config.oledMode == 3) {
        oledInterval = 45; // Ultra-fast scrolling text speed for announce page!
    } else if (configMgr.config.oledMode == 5 || notificationMgr.isOledActive()) {
        oledInterval = 100; // Smooth animations for mascot & notifications
    }
    if (currentMs - lastOledRefreshMs >= oledInterval) {
        lastOledRefreshMs = currentMs;
        timeStr = getFormattedNtpTime();
        int rssi = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : 0;
        
        oledMgr.draw(sensorMgr, notificationMgr, localIpStr, timeStr, rssi, configMgr.config.oledClockStyle);
        tftMgr.renderCurrentPage(weatherMgr.weather, sensorMgr, pomoTimer, ancsClientMgr.phoneLog, notificationMgr, localIpStr, timeStr);
    }
}
