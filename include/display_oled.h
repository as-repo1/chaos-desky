#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "config.h"
#include "sensors.h"

class OledDisplayManager {
public:
    OledDisplayManager() : oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {}

    bool begin() {
        if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
            Serial.println("❌ OLED SSD1306 Allocation Failed!");
            return false;
        }

        oled.clearDisplay();
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(15, 20);
        oled.println("CHAOS DESKY v1.0");
        oled.setCursor(20, 35);
        oled.println("Initializing...");
        oled.display();
        return true;
    }

    void drawHUD(const SensorData& s, const String& ipStr, const String& timeStr, int rssi) {
        oled.clearDisplay();

        // 1. Top Status Bar (0 - 11 px)
        oled.fillRect(0, 0, 128, 12, SSD1306_WHITE);
        oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        oled.setTextSize(1);

        // WiFi Icon / Signal
        oled.setCursor(2, 2);
        if (WiFi.status() == WL_CONNECTED) {
            oled.print("WIFI ");
        } else {
            oled.print("OFF  ");
        }

        // Time or IP string
        oled.setCursor(38, 2);
        if (timeStr.length() > 0 && timeStr != "00:00:00") {
            oled.print(timeStr);
        } else {
            oled.print(ipStr);
        }

        oled.setCursor(102, 2);
        if (WiFi.status() == WL_CONNECTED) {
            oled.printf("%ddB", rssi);
        } else {
            oled.print("DISC");
        }

        // Horizontal Separator Line
        oled.drawFastHLine(0, 13, 128, SSD1306_WHITE);
        oled.setTextColor(SSD1306_WHITE, SSD1306_BLACK);

        // 2. Indoor Climate Section (15 - 38 px)
        oled.setCursor(0, 16);
        oled.printf("TEMP: %.1f C", s.tempC);
        
        oled.setCursor(72, 16);
        oled.printf("HUM: %.0f%%", s.humidity);

        oled.setCursor(0, 27);
        oled.printf("PRESS:%.1fhPa", s.pressureHpa);

        oled.setCursor(72, 27);
        oled.printf("ALT:%.0fm", s.altitudeM);

        // Horizontal Divider Line
        oled.drawFastHLine(0, 37, 128, SSD1306_WHITE);

        // 3. Derived Comfort Metrics Section (39 - 63 px)
        oled.setCursor(0, 41);
        oled.printf("HI: %.1f C  DEW:%.1f C", s.heatIndexC, s.dewPointC);

        oled.setCursor(0, 52);
        oled.printf("IP: %s", ipStr.c_str());

        oled.display();
    }

private:
    Adafruit_SSD1306 oled;
};

#endif // DISPLAY_OLED_H
