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
    int oledMode = 0;
    uint8_t contrast = 255;
    bool isInverted = false;
    String customText = "Welcome to ChaosDesky!";
    int scrollX = 128;

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

    void setContrast(uint8_t val) {
        contrast = val;
        oled.ssd1306_command(SSD1306_SETCONTRAST);
        oled.ssd1306_command(val);
    }

    void setInverted(bool invert) {
        isInverted = invert;
        oled.invertDisplay(invert);
    }

    void draw(const SensorData& s, const String& ipStr, const String& timeStr, int rssi) {
        oled.clearDisplay();

        switch (oledMode) {
            case 0:
                drawMode0_HUD(s, ipStr, timeStr, rssi);
                break;
            case 1:
                drawMode1_BigClock(timeStr, ipStr);
                break;
            case 2:
                drawMode2_Sparklines(s);
                break;
            case 3:
                drawMode3_Marquee(customText);
                break;
            case 4:
                drawMode4_CustomImage();
                break;
            default:
                drawMode0_HUD(s, ipStr, timeStr, rssi);
                break;
        }

        oled.display();
    }

private:
    Adafruit_SSD1306 oled;

    // Mode 0: Telemetry HUD
    void drawMode0_HUD(const SensorData& s, const String& ipStr, const String& timeStr, int rssi) {
        // Status Bar
        oled.fillRect(0, 0, 128, 12, SSD1306_WHITE);
        oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        oled.setTextSize(1);

        oled.setCursor(2, 2);
        oled.print(WiFi.status() == WL_CONNECTED ? "WIFI " : "OFF  ");

        oled.setCursor(38, 2);
        oled.print(timeStr.length() > 0 && timeStr != "00:00:00" ? timeStr : ipStr);

        oled.setCursor(102, 2);
        oled.printf(WiFi.status() == WL_CONNECTED ? "%ddB" : "DISC", rssi);

        oled.drawFastHLine(0, 13, 128, SSD1306_WHITE);
        oled.setTextColor(SSD1306_WHITE, SSD1306_BLACK);

        // Sensor Grid
        oled.setCursor(0, 16);
        oled.printf("TEMP: %.1f C", s.tempC);
        oled.setCursor(72, 16);
        oled.printf("HUM: %.0f%%", s.humidity);

        oled.setCursor(0, 27);
        oled.printf("PRESS:%.1fhPa", s.pressureHpa);
        oled.setCursor(72, 27);
        oled.printf("ALT:%.0fm", s.altitudeM);

        oled.drawFastHLine(0, 37, 128, SSD1306_WHITE);

        oled.setCursor(0, 41);
        oled.printf("HI: %.1f C  DEW:%.1f C", s.heatIndexC, s.dewPointC);
        oled.setCursor(0, 52);
        oled.printf("IP: %s", ipStr.c_str());
    }

    // Mode 1: Large Digital Clock & Date
    void drawMode1_BigClock(const String& timeStr, const String& ipStr) {
        oled.setTextColor(SSD1306_WHITE);
        
        // Large Time Display
        oled.setTextSize(3);
        oled.setCursor(6, 12);
        oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : "00:00");

        // Seconds Ticker
        oled.setTextSize(1);
        oled.setCursor(102, 26);
        oled.print(timeStr.length() >= 8 ? timeStr.substring(6, 8) : "00");

        oled.drawFastHLine(0, 44, 128, SSD1306_WHITE);

        // IP / Network Footer
        oled.setCursor(4, 50);
        oled.printf("IP: %s", ipStr.c_str());
    }

    // Mode 2: Temp & Humidity Sparkline Graphs
    void drawMode2_Sparklines(const SensorData& s) {
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        
        oled.setCursor(0, 0);
        oled.printf("T:%.1fC  MIN:%.1f MAX:%.1f", s.tempC, s.minTempC, s.maxTempC);
        
        // Temperature Box (y: 12 to 32)
        oled.drawRect(0, 12, 128, 20, SSD1306_WHITE);
        oled.setCursor(2, 18);
        oled.print("TEMP OK");

        oled.setCursor(0, 34);
        oled.printf("H:%.0f%%  PRESS:%.1fhPa", s.humidity, s.pressureHpa);

        // Humidity Box (y: 44 to 64)
        oled.drawRect(0, 44, 128, 20, SSD1306_WHITE);
        oled.setCursor(2, 50);
        oled.print("HUMIDITY OK");
    }

    // Mode 3: Custom Text Scrolling Marquee
    void drawMode3_Marquee(const String& text) {
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(4, 2);
        oled.print("=== ANNOUNCEMENT ===");

        oled.drawFastHLine(0, 14, 128, SSD1306_WHITE);

        // Large Scrolling Banner Text
        oled.setTextSize(2);
        oled.setCursor(scrollX, 28);
        oled.print(text);

        scrollX -= 4;
        int textWidth = text.length() * 12;
        if (scrollX < -textWidth) {
            scrollX = 128;
        }

        oled.drawFastHLine(0, 52, 128, SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(4, 55);
        oled.print("CHAOSDESKY DESK HUB");
    }

    // Mode 4: Custom Uploaded Bitmap Image
    void drawMode4_CustomImage() {
        if (LittleFS.exists("/custom_oled.raw")) {
            File f = LittleFS.open("/custom_oled.raw", "r");
            if (f && f.size() >= 1024) {
                uint8_t buffer[1024];
                f.read(buffer, 1024);
                f.close();
                oled.drawBitmap(0, 0, buffer, 128, 64, SSD1306_WHITE, SSD1306_BLACK);
                return;
            }
        }
        
        // Fallback default graphics
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(10, 20);
        oled.print("NO OLED IMAGE UPLOADED");
        oled.setCursor(10, 38);
        oled.print("Upload via Web UI!");
    }
};

#endif // DISPLAY_OLED_H
