#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"
#include "sensors.h"
#include "weather_api.h"
#include "pomodoro.h"
#include "display_tft.h"

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

        // Serve Static LittleFS Web Files
        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

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
            doc["minTempC"]     = d.minTempC;
            doc["maxTempC"]     = d.maxTempC;
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
