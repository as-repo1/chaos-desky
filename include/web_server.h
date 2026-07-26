#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "config.h"
#include "sensors.h"
#include "weather_api.h"
#include "pomodoro.h"
#include "display_tft.h"

extern String localIpStr;
extern unsigned long carouselIntervalMs;

class WebServerManager {
public:
    WebServerManager(int port = 80) : server(port) {}

    void begin(SensorManager* sensors, 
               WeatherApiClient* weatherClient, 
               PomodoroTimer* pomo, 
               TftDisplayManager* tftMgr) {
        
        sensorMgr = sensors;
        weatherMgr = weatherClient;
        pomoTimer = pomo;
        tftManager = tftMgr;

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

        // REST API: POST /api/tft/page?page=0|1|2|3
        server.on("/api/tft/page", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("page")) {
                int p = request->getParam("page")->value().toInt();
                tftManager->setPage(p);
            }
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // REST API: POST /api/tft/theme?theme=0|1|2|3
        server.on("/api/tft/theme", HTTP_POST, [this](AsyncWebServerRequest* request) {
            if (request->hasParam("theme")) {
                int t = request->getParam("theme")->value().toInt();
                tftManager->applyTheme((TFTTheme)t);
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

        // REST API: POST /api/config (Update Weather City / API Key / Carousel timing)
        server.on("/api/config", HTTP_POST, [this](AsyncWebServerRequest* request) {
            String city = request->hasParam("city", true) ? request->getParam("city", true)->value() : "411057";
            String apiKey = request->hasParam("apiKey", true) ? request->getParam("apiKey", true)->value() : OPENWEATHER_API_KEY;

            if (request->hasParam("carouselSec", true)) {
                int sec = request->getParam("carouselSec", true)->value().toInt();
                carouselIntervalMs = (unsigned long)sec * 1000;
            }

            Serial.println("⚙️ Updating OpenWeather Config from Web UI:");
            Serial.println("   City Query: " + city);
            Serial.println("   API Key:    " + apiKey.substring(0, 4) + "..." + apiKey.substring(apiKey.length() > 4 ? apiKey.length() - 4 : 0));

            weatherMgr->fetchWeather(apiKey, city, "IN");
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        });

        // Serve Static LittleFS Web Files AFTER all /api handlers!
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
};

#endif // WEB_SERVER_H
