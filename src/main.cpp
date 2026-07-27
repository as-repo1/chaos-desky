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
    bleUartMgr.update();

    // ==============================================================
    // ⌨️ SWITCH 1 (D25) — PAGE NAVIGATION & SLIDESHOW TOGGLE
    // ==============================================================
    SwitchAction action1 = mechSwitch1Mgr.update();
    if (action1 == SWITCH_SINGLE_CLICK) {
        // Pure manual navigation: Step to Next Page & reset timer
        tftMgr.nextPage();
        lastCarouselMs = currentMs; 
        Serial.printf("⌨️ D25 Single Click: Manual Switch to Page %d\n", tftMgr.currentPage);
    } 
    else if (action1 == SWITCH_DOUBLE_CLICK) {
        // Toggle between Manual-Only and Auto-Slideshow (6 seconds)
        if (carouselIntervalMs == 0) {
            carouselIntervalMs = 6000;
            configMgr.config.carouselSpeedSec = 6;
            notificationMgr.trigger("Slideshow Mode", "Auto-Slideshow ON (6s)", NOTIF_INFO, NOTIF_TARGET_BOTH, 3);
            Serial.println("⌨️ D25 Double Click: Auto-Slideshow Enabled (6s)");
        } else {
            carouselIntervalMs = 0;
            configMgr.config.carouselSpeedSec = 0;
            notificationMgr.trigger("Slideshow Mode", "Manual Navigation Only", NOTIF_INFO, NOTIF_TARGET_BOTH, 3);
            Serial.println("⌨️ D25 Double Click: Manual-Only Mode Enabled");
        }
        configMgr.saveConfig();
    } 
    else if (action1 == SWITCH_LONG_PRESS) {
        // Cycle OLED Display Mode (HUD -> Clock -> Sparklines -> Marquee -> Cyber Cat)
        configMgr.config.oledMode = (configMgr.config.oledMode + 1) % 5;
        configMgr.saveConfig();
        Serial.printf("⌨️ D25 Long Press: Cycled OLED Mode to %d\n", configMgr.config.oledMode);
    }

    // ==============================================================
    // 🎮 SWITCH 2 (D26) — TFT INTERACTION & ACTION SWITCH
    // ==============================================================
    SwitchAction action2 = mechSwitch2Mgr.update();
    if (action2 == SWITCH_SINGLE_CLICK) {
        // Interact with current active TFT screen
        if (tftMgr.currentPage == 7) {
            // Watchface Studio Page -> Cycle through all 6 Watch Faces (including Casio!)
            watchFaceEngine.activeStyle = (watchFaceEngine.activeStyle + 1) % 6;
            tftMgr.forceRedraw();
            Serial.printf("🎮 D26 Single Click: Cycled Watch Face to %d\n", watchFaceEngine.activeStyle);
        } 
        else if (tftMgr.currentPage == 2) {
            // Pomodoro Page -> Toggle Start/Pause
            if (pomoTimer.state == POMO_IDLE || pomoTimer.state == POMO_PAUSED) {
                pomoTimer.startWork();
            } else {
                pomoTimer.pause();
            }
            tftMgr.forceRedraw();
            Serial.println("🎮 D26 Single Click: Toggled Pomodoro Timer!");
        } 
        else {
            // Other Dashboard Pages -> Cycle TFT Color Themes (11 Themes)
            configMgr.config.tftTheme = (configMgr.config.tftTheme + 1) % TOTAL_THEMES;
            configMgr.saveConfig();
            tftMgr.forceRedraw();
            Serial.printf("🎮 D26 Single Click: Cycled TFT Color Theme to %d\n", configMgr.config.tftTheme);
        }
    } 
    else if (action2 == SWITCH_DOUBLE_CLICK) {
        // Quick Jump to Pomodoro Screen & Toggle Timer
        tftMgr.setPage(2);
        if (pomoTimer.state == POMO_IDLE || pomoTimer.state == POMO_PAUSED) {
            pomoTimer.startWork();
        } else {
            pomoTimer.pause();
        }
        Serial.println("🎮 D26 Double Click: Jumped to Pomodoro & Toggled!");
    } 
    else if (action2 == SWITCH_LONG_PRESS) {
        // Instant Hotkey -> Snap directly to Iconic Casio F-91W Watchface!
        watchFaceEngine.activeStyle = WATCHFACE_CASIO_F91W;
        tftMgr.setPage(7);
        Serial.println("🎮 D26 Long Press: Jumped to Casio F-91W Watchface!");
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

    // 4. Periodic Weather API Fetch (Every 10 mins)
    if (currentMs - lastWeatherFetchMs >= WEATHER_UPDATE_MS) {
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
