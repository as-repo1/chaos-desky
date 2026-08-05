#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "config.h"
#include "config_manager.h"
#include "sensors.h"
#include "weather_api.h"
#include "pomodoro.h"
#include "display_oled.h"
#include "display_tft.h"
#include "notification_manager.h"

extern String localIpStr;
extern unsigned long carouselIntervalMs;
extern ConfigManager configMgr;
extern OledDisplayManager oledMgr;
extern NotificationManager notificationMgr;

class WebServerManager {
public:
    WebServerManager(int port = 80) : server(port) {}

    void begin(SensorManager* sensors, 
               WeatherApiClient* weatherClient, 
               PomodoroTimer* pomo, 
               TftDisplayManager* tftMgr,
               NotificationManager* notifMgr) {
        
        sensorMgr = sensors;
        weatherMgr = weatherClient;
        pomoTimer = pomo;
        tftManager = tftMgr;
        notificationEngine = notifMgr;

        // REST API: GET /api/sensors
        server.on("/api/sensors", HTTP_GET, [this](AsyncWebServerRequest* request) {
            StaticJsonDocument<512> doc;
            SensorData& d = sensorMgr->data;
            
            doc["tempC"]        = d.tempC;
            doc["tempF"]        = d.tempF;
            doc["humidity"]     = d.humidity;
            doc["heatIndexC"]   = d.heatIndexC;
            doc["dewPointC"]    = d.dewPointC;
            doc["pressureHpa"]  = d.pressureHpa;
            doc["altitudeM"]    = d.altitudeM;
            doc["minTempC"]     = (d.minTempC < 90.0f) ? d.minTempC : d.tempC;
            doc["maxTempC"]     = (d.maxTempC > -90.0f) ? d.maxTempC : d.tempC;
            doc["dhtValid"]     = d.dhtValid;
            doc["bmpValid"]     = d.bmpValid;

            String json;
            serializeJson(doc, json);
            request->send(200, "application/json", json);
        });

        // REST API: GET /api/weather
        server.on("/api/weather", HTTP_GET, [this](AsyncWebServerRequest* request) {
            StaticJsonDocument<512> doc;
            OutdoorWeatherData& w = weatherMgr->weather;

            doc["tempC"]        = w.tempC;
            doc["tempMinC"]     = w.tempMinC;
            doc["tempMaxC"]     = w.tempMaxC;
            doc["humidity"]     = w.humidity;
            doc["windSpeedMs"]  = w.windSpeedMs;
            doc["cityName"]     = w.cityName;
            doc["condition"]    = w.condition;
            doc["iconCode"]     = w.iconCode;
            doc["valid"]        = w.valid;

            String json;
            serializeJson(doc, json);
            request->send(200, "application/json", json);
        });

        // REST API: GET /api/pomodoro
        server.on("/api/pomodoro", HTTP_GET, [this](AsyncWebServerRequest* request) {
            StaticJsonDocument<256> doc;
            doc["state"]        = pomoTimer->getStateString();
            doc["timeString"]   = pomoTimer->getFormattedTime();
            doc["remainingSec"] = pomoTimer->remainingSec;
            doc["progress"]     = pomoTimer->getProgress();
            doc["sessions"]     = pomoTimer->completedSessions;
            doc["workMins"]     = pomoTimer->workDurationMins;
            doc["breakMins"]    = pomoTimer->breakDurationMins;

            String json;
            serializeJson(doc, json);
            request->send(200, "application/json", json);
        });

        // REST API: POST /api/pomodoro?action=start|pause|resume|reset|break
        server.on("/api/pomodoro", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("action")) {
                String act = request->getParam("action")->value();
                if (act == "start") pomoTimer->startWork();
                else if (act == "pause") pomoTimer->pause();
                else if (act == "resume") pomoTimer->resume();
                else if (act == "reset") pomoTimer->reset();
                else if (act == "break") pomoTimer->startBreak();
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/pomodoro/config?work=25&break=5
        server.on("/api/pomodoro/config", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("work", true)) {
                int w = request->getParam("work", true)->value().toInt();
                if (w > 0 && w <= 120) {
                    pomoTimer->workDurationMins = w;
                    configMgr.config.pomoWorkMins = w;
                }
            }
            if (request->hasParam("break", true)) {
                int b = request->getParam("break", true)->value().toInt();
                if (b > 0 && b <= 60) {
                    pomoTimer->breakDurationMins = b;
                    configMgr.config.pomoBreakMins = b;
                }
            }
            configMgr.saveConfig();
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/tft/page?page=0..4
        server.on("/api/tft/page", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("page")) {
                int p = request->getParam("page")->value().toInt();
                tftManager->setPage(p);
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/tft/watchface
        server.on("/api/tft/watchface", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("style", true)) {
                int st = request->getParam("style", true)->value().toInt();
                watchFaceEngine.activeStyle = st;
                tftManager->setPage(6); // Auto-jump to Watch Face Page (Index 6)
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/tft/theme?theme=0..10
        server.on("/api/tft/theme", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("theme")) {
                int t = request->getParam("theme")->value().toInt();
                if (t >= 0 && t < TOTAL_THEMES) {
                    tftManager->applyTheme((TFTTheme)t);
                    configMgr.config.tftTheme = t;
                    configMgr.saveConfig();
                }
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/tft/climatetheme?theme=0..6
        server.on("/api/tft/climatetheme", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("theme")) {
                int t = request->getParam("theme")->value().toInt();
                if (t >= 0 && t <= 6) { // 7 Themes Total
                    tftManager->activeClimateTheme = t;
                    tftManager->setPage(5); // Jump to climate page (Index 5)
                }
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/tft/rotation?rot=0..3
        server.on("/api/tft/rotation", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("rot")) {
                int r = request->getParam("rot")->value().toInt();
                tftManager->setRotation(r);
                configMgr.config.tftRotation = r;
                configMgr.saveConfig();
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/tft/pagemask?mask=31
        server.on("/api/tft/pagemask", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("mask")) {
                uint16_t m = (uint16_t)request->getParam("mask")->value().toInt();
                tftManager->enabledPagesMask = m;
                configMgr.config.enabledPagesMask = m;
                configMgr.saveConfig();
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/oled/mode?mode=0..5
        server.on("/api/oled/mode", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("mode")) {
                int m = request->getParam("mode")->value().toInt();
                oledMgr.oledMode = m;
                configMgr.config.oledMode = m;
                configMgr.saveConfig();
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/oled/clock-style?style=0..5
        server.on("/api/oled/clock-style", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("style")) {
                int s = request->getParam("style")->value().toInt();
                configMgr.config.oledClockStyle = s;
                configMgr.saveConfig();
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/oled/contrast?level=0..255
        server.on("/api/oled/contrast", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("level")) {
                int l = request->getParam("level")->value().toInt();
                oledMgr.setContrast((uint8_t)l);
                configMgr.config.oledContrast = (uint8_t)l;
                configMgr.saveConfig();
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/oled/text (Set Custom Scrolling Text)
        server.on("/api/oled/text", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("text", true)) {
                String txt = request->getParam("text", true)->value();
                oledMgr.customText = txt;
                tftManager->customBannerText = txt;
                configMgr.config.customText = txt;
                configMgr.saveConfig();
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: GET /api/wifi/scan
        server.on("/api/wifi/scan", HTTP_GET, [](AsyncWebServerRequest* request) {
            int n = WiFi.scanNetworks();
            StaticJsonDocument<1024> doc;
            JsonArray networks = doc.createNestedArray("networks");

            for (int i = 0; i < n; ++i) {
                JsonObject net = networks.createNestedObject();
                net["ssid"] = WiFi.SSID(i);
                net["rssi"] = WiFi.RSSI(i);
                net["enc"]  = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
            }

            String json;
            serializeJson(doc, json);
            request->send(200, "application/json", json);
        });

        // REST API: POST /api/config
        server.on("/api/config", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("city", true)) {
                configMgr.config.openWeatherCity = request->getParam("city", true)->value();
            }
            if (request->hasParam("apiKey", true)) {
                configMgr.config.openWeatherKey = request->getParam("apiKey", true)->value();
            }
            if (request->hasParam("carouselSec", true)) {
                int sec = request->getParam("carouselSec", true)->value().toInt();
                carouselIntervalMs = (unsigned long)sec * 1000;
                configMgr.config.carouselSpeedSec = sec;
            }

            configMgr.saveConfig();
            weatherMgr->fetchWeather(configMgr.config.openWeatherKey, configMgr.config.openWeatherCity, "IN");
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/optimize (Feature & Resource Optimization)
        server.on("/api/optimize", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("ble", true)) {
                configMgr.config.featureBleEnabled = (request->getParam("ble", true)->value() == "1");
            }
            if (request->hasParam("weather", true)) {
                configMgr.config.featureWeatherEnabled = (request->getParam("weather", true)->value() == "1");
            }
            if (request->hasParam("screensaver", true)) {
                configMgr.config.featureScreensaverEnabled = (request->getParam("screensaver", true)->value() == "1");
            }
            configMgr.saveConfig();
            notificationEngine->trigger("Optimization", "Resource & Feature Toggles Saved!", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 3);
            request->send(200, "application/json", "{\"status\":\"ok\",\"optimized\":true}");
        });

        // REST API: GET /api/notify/target (System Alerts Display Preference)
        server.on("/api/notify/target", HTTP_GET, [](AsyncWebServerRequest* request) {
            StaticJsonDocument<128> doc;
            doc["notifTarget"] = configMgr.config.notifTarget;
            String json;
            serializeJson(doc, json);
            request->send(200, "application/json", json);
        });

        // REST API: POST /api/notify/target (Save System Alerts Display Preference)
        server.on("/api/notify/target", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("target", true)) {
                configMgr.config.notifTarget = request->getParam("target", true)->value().toInt();
                notificationEngine->userPreference = (NotificationTarget)configMgr.config.notifTarget;
                configMgr.saveConfig();
                notificationEngine->trigger("Alert Target", "Display Preference Updated!", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 3);
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/notify (Trigger Display Notification Popup)
        server.on("/api/notify", HTTP_POST, [this](AsyncWebServerRequest* request) {
            String title = request->hasParam("title", true) ? request->getParam("title", true)->value() : "Alert";
            String msg = request->hasParam("message", true) ? request->getParam("message", true)->value() : "Notification Received";
            int cat = request->hasParam("category", true) ? request->getParam("category", true)->value().toInt() : 0;
            int target = request->hasParam("target", true) ? request->getParam("target", true)->value().toInt() : 2;
            int duration = request->hasParam("duration", true) ? request->getParam("duration", true)->value().toInt() : 8;

            notificationEngine->trigger(title, msg, (NotificationCategory)cat, (NotificationTarget)target, duration);
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: GET /api/todo (Fetch To-Do List)
        server.on("/api/todo", HTTP_GET, [this](AsyncWebServerRequest* request) {
            StaticJsonDocument<512> doc;
            JsonArray tT = doc.createNestedArray("titles");
            JsonArray tC = doc.createNestedArray("checked");
            for (int i = 0; i < 4; i++) {
                tT.add(tftManager->todoTitles[i]);
                tC.add(tftManager->todoChecked[i]);
            }
            doc["selected"] = tftManager->todoSelectedIdx;
            String json;
            serializeJson(doc, json);
            request->send(200, "application/json", json);
        });

        // REST API: POST /api/todo/update (Update To-Do List)
        server.on("/api/todo/update", HTTP_POST, [this](AsyncWebServerRequest* request) {
            bool modified = false;
            for (int i = 0; i < 4; i++) {
                String titleParam = "t" + String(i);
                String checkParam = "c" + String(i);
                if (request->hasParam(titleParam, true)) {
                    tftManager->todoTitles[i] = request->getParam(titleParam, true)->value().substring(0, 20);
                    configMgr.config.todoTitles[i] = tftManager->todoTitles[i];
                    modified = true;
                }
                if (request->hasParam(checkParam, true)) {
                    tftManager->todoChecked[i] = (request->getParam(checkParam, true)->value() == "1" || request->getParam(checkParam, true)->value() == "true");
                    configMgr.config.todoChecked[i] = tftManager->todoChecked[i];
                    modified = true;
                }
            }
            if (modified) {
                configMgr.saveConfig();
                tftManager->forceRedraw();
                notificationEngine->trigger("To-Do Target", "Task List Updated!", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 3);
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: GET /api/hid (Fetch HID Macros)
        server.on("/api/hid", HTTP_GET, [this](AsyncWebServerRequest* request) {
            StaticJsonDocument<256> doc;
            doc["hidMacroLeft"] = configMgr.config.hidMacroLeft;
            doc["hidMacroRight"] = configMgr.config.hidMacroRight;
            String json;
            serializeJson(doc, json);
            request->send(200, "application/json", json);
        });

        // REST API: POST /api/hid (Update HID Macros)
        server.on("/api/hid", HTTP_POST, [this](AsyncWebServerRequest* request) {
            bool modified = false;
            if (request->hasParam("left", true)) {
                configMgr.config.hidMacroLeft = request->getParam("left", true)->value();
                modified = true;
            }
            if (request->hasParam("right", true)) {
                configMgr.config.hidMacroRight = request->getParam("right", true)->value();
                modified = true;
            }
            if (modified) {
                configMgr.saveConfig();
                if (tftManager->currentPage == 13) {
                    tftManager->forceRedraw();
                }
                notificationEngine->trigger("HID Macros", "Macros Updated!", NOTIF_INFO, NOTIF_TARGET_USER_PREF, 3);
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // Serve Static LittleFS Files AFTER all /api handlers!
        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

        server.begin();
        Serial.println("✅ Embedded Async Web Server Started!");
    }

private:
    AsyncWebServer server;
    SensorManager* sensorMgr = nullptr;
    WeatherApiClient* weatherMgr = nullptr;
    PomodoroTimer* pomoTimer = nullptr;
    TftDisplayManager* tftManager = nullptr;
    NotificationManager* notificationEngine = nullptr;
};

#endif // WEB_SERVER_H
