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
    String cityName = "Hinjewadi";
    String condition = "Unknown";
    String iconCode = "01d";
    bool valid = false;
    unsigned long lastFetchMs = 0;
};

class WeatherApiClient {
public:
    OutdoorWeatherData weather;

    static String urlEncode(const String& str) {
        String encoded = "";
        for (size_t i = 0; i < str.length(); i++) {
            char c = str.charAt(i);
            if (c == ' ') {
                encoded += "%20";
            } else if (c == ',') {
                encoded += "%2C";
            } else if (isalnum(c) || c == '-' || c == '_' || c == '.') {
                encoded += c;
            } else {
                char buf[4];
                snprintf(buf, sizeof(buf), "%%%02X", (unsigned char)c);
                encoded += buf;
            }
        }
        return encoded;
    }

    static String extract6DigitPin(const String& str) {
        int count = 0;
        String pin = "";
        for (size_t i = 0; i < str.length(); i++) {
            char c = str.charAt(i);
            if (isdigit(c)) {
                count++;
                pin += c;
                if (count == 6) return pin;
            } else {
                count = 0;
                pin = "";
            }
        }
        return "";
    }

    bool fetchWeather(const String& apiKey, const String& cityQuery, const String& country = "IN") {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("⚠️ OpenWeather API: WiFi Not Connected!");
            weather.valid = false;
            return false;
        }

        if (apiKey.isEmpty() || apiKey == "YOUR_OPENWEATHER_API_KEY") {
            Serial.println("⚠️ OpenWeather API Key not configured!");
            weather.cityName = "Hinjewadi";
            weather.condition = "Configure API Key";
            weather.valid = false;
            return false;
        }

        String targetInput = cityQuery.isEmpty() ? "411057" : cityQuery;
        String pinCode = extract6DigitPin(targetInput);
        String url;

        if (pinCode.length() == 6) {
            // Query by 6-digit Indian Postal PIN Code (e.g. 411057 for Hinjewadi Phase 1)
            url = "http://api.openweathermap.org/data/2.5/weather?zip=" + pinCode + "," + country + 
                  "&units=metric&appid=" + apiKey;
        } else {
            // Query by City Name (e.g. Hinjewadi or Pune)
            url = "http://api.openweathermap.org/data/2.5/weather?q=" + urlEncode(targetInput) + "," + country + 
                  "&units=metric&appid=" + apiKey;
        }

        Serial.println("🌐 OpenWeather HTTP GET Request: " + url);

        HTTPClient http;
        http.begin(url);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                weather.tempC       = doc["main"]["temp"] | 0.0f;
                weather.tempMinC    = doc["main"]["temp_min"] | 0.0f;
                weather.tempMaxC    = doc["main"]["temp_max"] | 0.0f;
                weather.humidity    = doc["main"]["humidity"] | 0.0f;
                weather.windSpeedMs = doc["wind"]["speed"] | 0.0f;
                
                String rawName      = doc["name"] | "Hinjewadi";
                weather.cityName    = rawName;
                
                JsonObject wObj     = doc["weather"][0];
                weather.condition   = wObj["main"].as<String>();
                weather.iconCode    = wObj["icon"].as<String>();
                
                weather.valid = true;
                weather.lastFetchMs = millis();
                http.end();
                Serial.printf("✅ OpenWeather API Success for %s: %.1f°C, %s\n", 
                              weather.cityName.c_str(), weather.tempC, weather.condition.c_str());
                return true;
            } else {
                Serial.println("❌ OpenWeather JSON Parse Error: " + String(error.c_str()));
            }
        } else {
            Serial.printf("❌ OpenWeather HTTP Response Code: %d\n", httpCode);
            String errResponse = http.getString();
            Serial.println("❌ Response: " + errResponse);

            // Fallback: If 411057 or custom text failed, retry with "Pune"
            if (targetInput != "Pune") {
                Serial.println("🔄 Retrying OpenWeather API with Fallback City: Pune...");
                http.end();
                return fetchWeather(apiKey, "Pune", country);
            }
        }

        http.end();
        weather.valid = false;
        return false;
    }
};

#endif // WEATHER_API_H
