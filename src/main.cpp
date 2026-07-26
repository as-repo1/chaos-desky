#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <LittleFS.h>

#include "config.h"
#include "sensors.h"
#include "zambretti.h"
#include "weather_api.h"
#include "pomodoro.h"
#include "display_oled.h"
#include "display_tft.h"
#include "web_server.h"

// System Instances
SensorManager       sensorMgr;
WeatherApiClient    weatherMgr;
PomodoroTimer       pomoTimer;
OledDisplayManager  oledMgr;
TftDisplayManager   tftMgr;
WebServerManager    webServerMgr(80);

// Timing Variables
unsigned long lastSensorReadMs  = 0;
unsigned long lastWeatherFetchMs = 0;
unsigned long lastCarouselMs     = 0;
unsigned long lastOledRefreshMs  = 0;
unsigned long lastPressureSampleMs = 0;

String localIpStr = "0.0.0.0";
String timeStr    = "00:00:00";

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
    Serial.println("🌐 Connecting to WiFi: " + String(DEFAULT_WIFI_SSID));
    WiFi.mode(WIFI_STA);
    WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASS);

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

    // Initialize Displays
    oledMgr.begin();
    tftMgr.begin();

    // Initialize Sensors
    sensorMgr.begin();

    // Setup Network & Web Server
    setupWiFi();
    webServerMgr.begin(&sensorMgr, &weatherMgr, &pomoTimer, &tftMgr);

    // Fetch initial Outdoor Weather
    weatherMgr.fetchWeather(OPENWEATHER_API_KEY, OPENWEATHER_CITY, OPENWEATHER_COUNTRY);

    // Add initial barometric pressure sample
    sensorMgr.addPressureSample(sensorMgr.data.pressureHpa);

    Serial.println("🎉 System Initialization Complete!");
}

void loop() {
    unsigned long currentMs = millis();

    // 1. Update Pomodoro State Machine
    pomoTimer.update();

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
        weatherMgr.fetchWeather(OPENWEATHER_API_KEY, OPENWEATHER_CITY, OPENWEATHER_COUNTRY);
    }

    // 5. TFT Carousel Page Switcher (Every 10 seconds)
    if (CAROUSEL_INTERVAL_MS > 0 && (currentMs - lastCarouselMs >= CAROUSEL_INTERVAL_MS)) {
        lastCarouselMs = currentMs;
        tftMgr.nextPage();
    }

    // 6. Update OLED HUD (Every 500 ms)
    if (currentMs - lastOledRefreshMs >= 500) {
        lastOledRefreshMs = currentMs;
        timeStr = getFormattedNtpTime();
        int rssi = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : 0;
        
        oledMgr.drawHUD(sensorMgr.data, localIpStr, timeStr, rssi);

        // Update TFT Current Page
        tftMgr.renderCurrentPage(weatherMgr.weather, sensorMgr, pomoTimer, localIpStr, timeStr);
    }
}
