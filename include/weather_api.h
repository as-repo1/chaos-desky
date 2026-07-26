#ifndef WEATHER_API_H
#define WEATHER_API_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

struct OutdoorWeatherData {
    float tempC = 0.0f;
    float tempMinC = 0.0f;
    float tempMaxC = 0.0f;
    float humidity = 0.0f;
    float windSpeedMs = 0.0f;
    String cityName = "N/A";
    String condition = "Unknown";
    String iconCode = "01d";
    bool valid = false;
    unsigned long lastFetchMs = 0;
};

class WeatherApiClient {
public:
    OutdoorWeatherData weather;

    bool fetchWeather(const String& apiKey, const String& city, const String& country) {
        if (WiFi.status() != WL_CONNECTED) {
            weather.valid = false;
            return false;
        }

        if (apiKey.isEmpty() || apiKey == "YOUR_OPENWEATHER_API_KEY") {
            Serial.println("⚠️ OpenWeather API Key not configured!");
            weather.cityName = city;
            weather.condition = "Configure API Key";
            weather.valid = false;
            return false;
        }

        HTTPClient http;
        String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + country + 
                     "&units=metric&appid=" + apiKey;

        http.begin(url);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                weather.tempC = doc["main"]["temp"] | 0.0f;
                weather.tempMinC = doc["main"]["temp_min"] | 0.0f;
                weather.tempMaxC = doc["main"]["temp_max"] | 0.0f;
                weather.humidity = doc["main"]["humidity"] | 0.0f;
                weather.windSpeedMs = doc["wind"]["speed"] | 0.0f;
                weather.cityName = doc["name"] | city;
                
                JsonObject wObj = doc["weather"][0];
                weather.condition = wObj["main"].as<String>();
                weather.iconCode = wObj["icon"].as<String>();
                
                weather.valid = true;
                weather.lastFetchMs = millis();
                http.end();
                Serial.println("✅ OpenWeather API fetch successful for: " + weather.cityName);
                return true;
            } else {
                Serial.println("❌ JSON Parse Error: " + String(error.c_str()));
            }
        } else {
            Serial.printf("❌ OpenWeather HTTP Error: %d\n", httpCode);
        }

        http.end();
        weather.valid = false;
        return false;
    }
};

#endif // WEATHER_API_H
