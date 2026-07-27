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
    // ⬅️ LEFT KEY (D25) — OLED CONTROL & TO-DO / NOTES JUMP
    // ==============================================================
    SwitchAction action1 = mechSwitch1Mgr.update();
    if (action1 == SWITCH_SINGLE_CLICK) {
        // Single Click -> Switches pages/modes on the OLED display!
        configMgr.config.oledMode = (configMgr.config.oledMode + 1) % 5;
        configMgr.saveConfig();
        Serial.printf("⬅️ D25 (Left) Single Click: Cycled OLED Mode to %d\n", configMgr.config.oledMode);
    } 
    else if (action1 == SWITCH_DOUBLE_CLICK) {
        // Double Click -> Jump to To-Do List (Page 4) & Notes / Notifs (Page 6) on the TFT!
        if (tftMgr.currentPage == 4) {
            tftMgr.setPage(6); // Toggle to Notes / Notifs Log
            Serial.println("⬅️ D25 (Left) Double Click: Jumped to Notes / Notifs Page!");
        } else {
            tftMgr.setPage(4); // Snap straight to To-Do / Custom User Notes Board
            Serial.println("⬅️ D25 (Left) Double Click: Jumped to To-Do & Notes Page!");
        }
    } 
    else if (action1 == SWITCH_LONG_PRESS) {
        // Current Default Long Press -> Toggle Pomodoro Timer Work/Pause & Quick-Jump to Pomodoro (P2)
        tftMgr.setPage(2);
        if (pomoTimer.state == POMO_IDLE || pomoTimer.state == POMO_PAUSED) {
            pomoTimer.startWork();
            notificationMgr.trigger("Pomodoro Timer", "Work Timer Started (25m)!", NOTIF_INFO, NOTIF_TARGET_BOTH, 3);
        } else {
            pomoTimer.pause();
            notificationMgr.trigger("Pomodoro Timer", "Timer Paused", NOTIF_WARNING, NOTIF_TARGET_BOTH, 3);
        }
        Serial.println("⬅️ D25 (Left) Long Press: Toggled Pomodoro Timer!");
    }

    // ==============================================================
    // ➡️ RIGHT KEY (D26) — TFT NAVIGATION & WATCHFACE CONTROL
    // ==============================================================
    SwitchAction action2 = mechSwitch2Mgr.update();
    if (action2 == SWITCH_SINGLE_CLICK) {
        // Single Click -> Switches pages manually on the TFT display!
        tftMgr.nextPage();
        lastCarouselMs = currentMs; // Reset timer if auto-slideshow is active
        Serial.printf("➡️ D26 (Right) Single Click: Manual Switch to TFT Page %d\n", tftMgr.currentPage);
    } 
    else if (action2 == SWITCH_DOUBLE_CLICK) {
        // Double Click -> Switch OLED to Clock Face (Mode 1) & cycle clock faces on both displays!
        configMgr.config.oledMode = 1; // Set OLED to Big Clock Mode
        configMgr.config.oledClockStyle = (configMgr.config.oledClockStyle + 1) % 6; // Cycle all 6 OLED Clock Faces!
        configMgr.saveConfig();
        
        if (tftMgr.currentPage != 7) {
            tftMgr.setPage(7); // Jump straight to Watchface Studio (Page 7)
            Serial.println("➡️ D26 (Right) Double Click: Switched OLED to Clock & TFT to Watchfaces!");
        } else {
            watchFaceEngine.activeStyle = (watchFaceEngine.activeStyle + 1) % 10; // Cycle all 10 Iconic Watch Faces!
            tftMgr.forceRedraw();
            Serial.printf("➡️ D26 (Right) Double Click: Cycled TFT to %d & OLED Clock to %d\n", watchFaceEngine.activeStyle, configMgr.config.oledClockStyle);
        }
    } 
    else if (action2 == SWITCH_LONG_PRESS) {
        // Current Default Long Press -> Cycle through all 11 TFT Color Themes & snap to Casio F-91W!
        if (tftMgr.currentPage == 7) {
            watchFaceEngine.activeStyle = WATCHFACE_CASIO_F91W;
            tftMgr.forceRedraw();
            notificationMgr.trigger("Casio F-91W", "Iconic Watchface Active!", NOTIF_INFO, NOTIF_TARGET_BOTH, 2);
        } else {
            configMgr.config.tftTheme = (configMgr.config.tftTheme + 1) % TOTAL_THEMES;
            configMgr.saveConfig();
            tftMgr.forceRedraw();
            notificationMgr.trigger("TFT Color Theme", "Theme Cycled!", NOTIF_INFO, NOTIF_TARGET_TFT, 2);
        }
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

    // 6. Update Displays (Every 500 ms)
    if (currentMs - lastOledRefreshMs >= 500) {
        lastOledRefreshMs = currentMs;
        timeStr = getFormattedNtpTime();
        int rssi = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : 0;
        
        oledMgr.draw(sensorMgr, notificationMgr, localIpStr, timeStr, rssi);
        tftMgr.renderCurrentPage(weatherMgr.weather, sensorMgr, pomoTimer, ancsClientMgr.phoneLog, notificationMgr, localIpStr, timeStr);
    }
}
