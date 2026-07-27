#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "config.h"
#include "sensors.h"
#include "notification_manager.h"
#include "screensaver.h"

extern ScreensaverEngine screensaverEngine;

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

    void draw(SensorManager& sm, const NotificationManager& notifMgr, const String& ipStr, const String& timeStr, int rssi) {
        oled.clearDisplay();

        if (notifMgr.isOledActive()) {
            drawNotificationOverlay(notifMgr.currentNotif, notifMgr.getProgress());
        } else {
            switch (oledMode) {
                case 0: drawMode0_HUD(sm.data, ipStr, timeStr, rssi); break;
                case 1: drawMode1_BigClock(timeStr, ipStr, sm.data); break;
                case 2: drawMode2_Sparklines(sm); break;
                case 3: drawMode3_Marquee(customText); break;
                case 4: drawMode4_CustomImage(); break;
                case 5: screensaverEngine.renderOledScreensaver(oled); break;
                default: drawMode0_HUD(sm.data, ipStr, timeStr, rssi); break;
            }
        }

        oled.display();
    }

private:
    Adafruit_SSD1306 oled;

    void drawNotificationOverlay(const NotificationItem& n, float progress) {
        oled.fillRoundRect(0, 0, 128, 14, 3, SSD1306_WHITE);
        oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(4, 3);

        switch (n.category) {
            case NOTIF_INFO:    oled.print("INFO ALERT"); break;
            case NOTIF_MESSAGE: oled.print("NEW MESSAGE"); break;
            case NOTIF_CALL:    oled.print("INCOMING CALL"); break;
            case NOTIF_WARNING: oled.print("WARNING!"); break;
            case NOTIF_ALERT:   oled.print("CRITICAL ALERT"); break;
        }

        oled.setTextColor(SSD1306_WHITE);
        oled.setCursor(4, 18);
        oled.print(n.title.substring(0, 20));

        oled.setCursor(4, 32);
        oled.print(n.message.substring(0, 20));

        if (n.message.length() > 20) {
            oled.setCursor(4, 44);
            oled.print(n.message.substring(20, 40));
        }
        // Rounded Progress bar
        oled.drawRoundRect(0, 56, 128, 8, 3, SSD1306_WHITE);
        int fillW = (int)((1.0f - progress) * 124.0f);
        if (fillW > 0) {
            oled.fillRoundRect(2, 58, fillW, 4, 2, SSD1306_WHITE);
        }
    }

    // Mode 0: Telemetry HUD
    void drawMode0_HUD(const SensorData& s, const String& ipStr, const String& timeStr, int rssi) {
        // Top Status Bar
        oled.drawFastHLine(0, 12, 128, SSD1306_WHITE);
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(2, 2);
        oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : "00:00");

        oled.setCursor(44, 2);
        oled.print(WiFi.status() == WL_CONNECTED ? "ONLINE" : "OFFLINE");

        oled.setCursor(96, 2);
        oled.printf("%ddB", rssi);

        // 2-Column Sensor Data Grid with vertical separator
        oled.drawFastVLine(64, 13, 38, SSD1306_WHITE);

        oled.setCursor(2, 16);
        oled.printf("TMP:%.1fC", s.tempC);
        oled.setCursor(68, 16);
        oled.printf("HUM:%.0f%%", s.humidity);

        oled.setCursor(2, 28);
        oled.printf("PRS:%.0fh", s.pressureHpa);
        oled.setCursor(68, 28);
        oled.printf("ALT:%.0fm", s.altitudeM);

        oled.drawFastHLine(0, 39, 128, SSD1306_WHITE);

        oled.setCursor(2, 42);
        oled.printf("HI:%.1fC  DEW:%.1fC", s.heatIndexC, s.dewPointC);
        oled.setCursor(2, 54);
        oled.printf("IP: %s", ipStr.c_str());
    }

    void drawMode1_BigClock(const String& timeStr, const String& ipStr, const SensorData& s) {
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);

        // Top Status Header
        oled.setCursor(2, 2);
        oled.print("SYSTEM CLOCK");
        oled.setCursor(80, 2);
        oled.printf("%.1fC", s.tempC);
        oled.drawFastHLine(0, 12, 128, SSD1306_WHITE);

        // Big Time Display
        oled.setTextSize(3);
        oled.setCursor(12, 18);
        oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : "00:00");

        // Small Seconds on right
        oled.setTextSize(1);
        oled.setCursor(104, 32);
        oled.print(timeStr.length() >= 8 ? timeStr.substring(6, 8) : "00");

        oled.drawFastHLine(0, 48, 128, SSD1306_WHITE);
        oled.setCursor(4, 53);
        oled.printf("WEB: %s", ipStr.c_str());
    }

    void drawMode2_Sparklines(SensorManager& sm) {
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);

        // Top Sparkline: Pressure History
        oled.setCursor(0, 0);
        oled.printf("PRESS: %.1fhPa", sm.data.pressureHpa);
        oled.drawRoundRect(0, 12, 128, 20, 3, SSD1306_WHITE);

        if (sm.historyCount > 1) {
            float minP = 1200.0f, maxP = 0.0f;
            for (int i = 0; i < sm.historyCount; i++) {
                if (sm.pressureHistory[i] < minP) minP = sm.pressureHistory[i];
                if (sm.pressureHistory[i] > maxP) maxP = sm.pressureHistory[i];
            }
            if (maxP - minP < 1.0f) { maxP += 0.5f; minP -= 0.5f; }

            int step = 124 / (sm.historyCount - 1);
            for (int i = 0; i < sm.historyCount - 1; i++) {
                int x1 = 2 + (i * step);
                int y1 = 30 - (int)(((sm.pressureHistory[i] - minP) / (maxP - minP)) * 16.0f);
                int x2 = 2 + ((i + 1) * step);
                int y2 = 30 - (int)(((sm.pressureHistory[i + 1] - minP) / (maxP - minP)) * 16.0f);
                oled.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
            }
        }

        // Bottom Sparkline: Indoor Temperature
        oled.setCursor(0, 34);
        oled.printf("TEMP: %.1fC (%.0f%% HUM)", sm.data.tempC, sm.data.humidity);
        oled.drawRoundRect(0, 44, 128, 20, 3, SSD1306_WHITE);
        oled.setCursor(4, 50);
        oled.print("INDOOR CLIMATE STABLE");
    }

    void drawMode3_Marquee(const String& text) {
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);

        // Draw clean speaker icon instead of emoji
        oled.fillRect(4, 3, 4, 6, SSD1306_WHITE);
        oled.fillTriangle(8, 3, 13, 0, 13, 11, SSD1306_WHITE);
        oled.drawPixel(15, 3, SSD1306_WHITE);
        oled.drawPixel(16, 5, SSD1306_WHITE);
        oled.drawPixel(15, 8, SSD1306_WHITE);

        oled.setCursor(22, 2);
        oled.print("ANNOUNCEMENT");
        oled.drawFastHLine(0, 13, 128, SSD1306_WHITE);

        // Disable text wrap to prevent scrolling text from spilling over multiple lines!
        oled.setTextWrap(false);
        oled.setTextSize(2);
        oled.setCursor(scrollX, 26);
        oled.print(text);
        oled.setTextWrap(true);

        scrollX -= 2;
        int textWidth = text.length() * 12;
        if (scrollX < -textWidth) {
            scrollX = 128;
        }

        oled.drawFastHLine(0, 50, 128, SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(10, 54);
        oled.print("CHAOSDESKY DESK HUB");
    }

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
        
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(10, 20);
        oled.print("NO OLED IMAGE UPLOADED");
        oled.setCursor(10, 38);
        oled.print("Upload via Web UI!");
    }
};

#endif // DISPLAY_OLED_H
