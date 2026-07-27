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

    void draw(SensorManager& sm, const NotificationManager& notifMgr, const String& ipStr, const String& timeStr, int rssi, int clockStyle = 0) {
        bool needsRefresh = false;
        // Only refresh I2C display if animation is playing, mode changed, clock style changed, or second/temp ticked!
        if (notifMgr.isOledActive() || oledMode == 3 || oledMode == 5 || timeStr != lastTimeStr || oledMode != lastRenderedMode || clockStyle != lastClockStyle || abs(sm.data.tempC - lastTemp) > 0.1f) {
            needsRefresh = true;
        }

        if (!needsRefresh) {
            return; // 🛡️ ZERO FLICKER: Eliminate redundant screen wiping & I2C writes
        }

        lastTimeStr = timeStr;
        lastRenderedMode = oledMode;
        lastClockStyle = clockStyle;
        lastTemp = sm.data.tempC;

        oled.clearDisplay();

        if (notifMgr.isOledActive()) {
            drawNotificationOverlay(notifMgr.currentNotif, notifMgr.getProgress());
        } else {
            switch (oledMode) {
                case 0: drawMode0_HUD(sm.data, ipStr, timeStr, rssi); break;
                case 1: drawMode1_BigClock(timeStr, ipStr, sm.data, clockStyle); break;
                case 2: drawMode2_Sparklines(sm); break;
                case 3: drawMode3_Marquee(customText); break;
                case 4: drawMode4_CustomImage(); break;
                case 5: screensaverEngine.renderOledScreensaver(oled); break;
                case 6: drawMode6_WifiInfo(ipStr, AP_SSID, AP_PASS); break;
                default: drawMode0_HUD(sm.data, ipStr, timeStr, rssi); break;
            }
        }

        oled.display();
    }

private:
    Adafruit_SSD1306 oled;
    String lastTimeStr = "";
    int lastRenderedMode = -1;
    int lastClockStyle = -1;
    float lastTemp = -999.0f;

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

    // Mode 1: Multi-Face OLED Clock Engine
    void drawMode1_BigClock(const String& timeStr, const String& ipStr, const SensorData& s, int clockStyle = 0) {
        switch (clockStyle) {
            case 0: drawOledClock_DigitalHUD(timeStr, ipStr, s); break;
            case 1: drawOledClock_AnalogMinimal(timeStr, s); break;
            case 2: drawOledClock_CyberMatrix(timeStr, s); break;
            case 3: drawOledClock_RetroFlip(timeStr, s); break;
            case 4: drawOledClock_VerticalStack(timeStr, s); break;
            case 5: drawOledClock_BinaryGauges(timeStr, s); break;
            case 6: drawOledClock_CyberpunkBox(timeStr, s); break;
            case 7: drawOledClock_RadialHorizon(timeStr, s); break;
            default: drawOledClock_DigitalHUD(timeStr, ipStr, s); break;
        }
    }

    // --- OLED Clock Style 0: Digital HUD Clock ---
    void drawOledClock_DigitalHUD(const String& timeStr, const String& ipStr, const SensorData& s) {
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(2, 2);
        oled.print("DESK CLOCK");
        oled.setCursor(76, 2);
        oled.printf("TMP:%.1fC", s.tempC);
        oled.drawFastHLine(0, 12, 128, SSD1306_WHITE);

        oled.setTextSize(3);
        oled.setCursor(8, 18);
        oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : "00:00");

        oled.setTextSize(1);
        oled.setCursor(100, 32);
        oled.print(timeStr.length() >= 8 ? timeStr.substring(6, 8) : "00");

        oled.drawFastHLine(0, 48, 128, SSD1306_WHITE);
        oled.setCursor(4, 53);
        oled.printf("IP: %s", ipStr.c_str());
    }

    // --- OLED Clock Style 1: Analog Minimalist Clock ---
    void drawOledClock_AnalogMinimal(const String& timeStr, const SensorData& s) {
        int cx = 32, cy = 32, r = 26;
        oled.drawCircle(cx, cy, r, SSD1306_WHITE);
        oled.fillCircle(cx, cy, 2, SSD1306_WHITE);

        // Ticks at 12, 3, 6, 9
        oled.drawFastVLine(32, 8, 4, SSD1306_WHITE);
        oled.drawFastVLine(32, 52, 4, SSD1306_WHITE);
        oled.drawFastHLine(8, 32, 4, SSD1306_WHITE);
        oled.drawFastHLine(52, 32, 4, SSD1306_WHITE);

        int h = 0, m = 0, sec = 0;
        if (timeStr.length() >= 8) {
            h = timeStr.substring(0, 2).toInt();
            m = timeStr.substring(3, 5).toInt();
            sec = timeStr.substring(6, 8).toInt();
        }

        // Hour Hand
        float angleH = ((h % 12) + m / 60.0f) * (2.0f * M_PI / 12.0f) - (M_PI / 2.0f);
        int hx = cx + (int)(cos(angleH) * 14);
        int hy = cy + (int)(sin(angleH) * 14);
        oled.drawLine(cx, cy, hx, hy, SSD1306_WHITE);

        // Minute Hand
        float angleM = (m + sec / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        int mx = cx + (int)(cos(angleM) * 20);
        int my = cy + (int)(sin(angleM) * 20);
        oled.drawLine(cx, cy, mx, my, SSD1306_WHITE);

        // Second Hand
        float angleS = sec * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        int sx = cx + (int)(cos(angleS) * 23);
        int sy = cy + (int)(sin(angleS) * 23);
        oled.drawLine(cx, cy, sx, sy, SSD1306_WHITE);

        // Right side Digital Info
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(2);
        oled.setCursor(64, 10);
        oled.printf("%02d:%02d", h, m);

        oled.setTextSize(1);
        oled.setCursor(66, 32);
        oled.printf("SEC : %02d", sec);
        oled.setCursor(66, 46);
        oled.printf("TEMP:%.1fC", s.tempC);
    }

    // --- OLED Clock Style 2: Cyber Matrix Clock ---
    void drawOledClock_CyberMatrix(const String& timeStr, const SensorData& s) {
        // High-tech corner brackets
        oled.drawFastHLine(0, 0, 12, SSD1306_WHITE);
        oled.drawFastVLine(0, 0, 12, SSD1306_WHITE);
        oled.drawFastHLine(116, 0, 12, SSD1306_WHITE);
        oled.drawFastVLine(127, 0, 12, SSD1306_WHITE);
        oled.drawFastHLine(0, 63, 12, SSD1306_WHITE);
        oled.drawFastVLine(0, 52, 12, SSD1306_WHITE);
        oled.drawFastHLine(116, 63, 12, SSD1306_WHITE);
        oled.drawFastVLine(127, 52, 12, SSD1306_WHITE);

        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(18, 4);
        oled.print("[ CYBER MATRIX ]");

        oled.setTextSize(3);
        oled.setCursor(14, 20);
        oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : "00:00");

        oled.drawRect(102, 22, 20, 18, SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(106, 26);
        oled.print(timeStr.length() >= 8 ? timeStr.substring(6, 8) : "00");

        oled.setCursor(12, 48);
        oled.printf("TMP:%.1fC  HUM:%.0f%%", s.tempC, s.humidity);
    }

    // --- OLED Clock Style 3: Retro Airport Flip Clock ---
    void drawOledClock_RetroFlip(const String& timeStr, const SensorData& s) {
        // Hours Card
        oled.drawRoundRect(4, 4, 56, 44, 5, SSD1306_WHITE);
        oled.drawFastHLine(4, 26, 56, SSD1306_WHITE);

        // Minutes Card
        oled.drawRoundRect(68, 4, 56, 44, 5, SSD1306_WHITE);
        oled.drawFastHLine(68, 26, 56, SSD1306_WHITE);

        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(3);
        oled.setCursor(14, 15);
        oled.print(timeStr.length() >= 2 ? timeStr.substring(0, 2) : "00");

        oled.setCursor(78, 15);
        oled.print(timeStr.length() >= 5 ? timeStr.substring(3, 5) : "00");

        // Blink Dots
        int sec = timeStr.length() >= 8 ? timeStr.substring(6, 8).toInt() : 0;
        if (sec % 2 == 0) {
            oled.fillCircle(64, 18, 2, SSD1306_WHITE);
            oled.fillCircle(64, 34, 2, SSD1306_WHITE);
        }

        // Footer Pill
        oled.drawRoundRect(8, 52, 112, 12, 3, SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(14, 54);
        oled.printf("SEC: %02d  |  %.1fC", sec, s.tempC);
    }

    // --- OLED Clock Style 4: Vertical Stack Clock ---
    void drawOledClock_VerticalStack(const String& timeStr, const SensorData& s) {
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(3);
        oled.setCursor(10, 4);
        oled.print(timeStr.length() >= 2 ? timeStr.substring(0, 2) : "00");

        oled.setCursor(10, 34);
        oled.print(timeStr.length() >= 5 ? timeStr.substring(3, 5) : "00");

        oled.drawFastVLine(62, 0, 64, SSD1306_WHITE);

        int sec = timeStr.length() >= 8 ? timeStr.substring(6, 8).toInt() : 0;
        oled.setTextSize(1);
        oled.setCursor(68, 4);
        oled.print("DESKY");

        oled.setTextSize(2);
        oled.setCursor(68, 18);
        oled.printf(":%02d", sec);

        oled.setTextSize(1);
        oled.setCursor(68, 40);
        oled.printf("T:%.1fC", s.tempC);
        oled.setCursor(68, 52);
        oled.printf("H:%.0f%%", s.humidity);
    }

    // --- OLED Clock Style 5: Binary Segment Bar Clock ---
    void drawOledClock_BinaryGauges(const String& timeStr, const SensorData& s) {
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(2);
        oled.setCursor(16, 2);
        oled.print(timeStr.length() >= 8 ? timeStr : "00:00:00");

        oled.drawFastHLine(0, 22, 128, SSD1306_WHITE);

        int h = 0, m = 0, sec = 0;
        if (timeStr.length() >= 8) {
            h = timeStr.substring(0, 2).toInt();
            m = timeStr.substring(3, 5).toInt();
            sec = timeStr.substring(6, 8).toInt();
        }

        // 6 Binary / Segment Bar Gauges for H1, H2, M1, M2, S1, S2
        int digits[6] = { h / 10, h % 10, m / 10, m % 10, sec / 10, sec % 10 };
        for (int i = 0; i < 6; i++) {
            int x = 6 + i * 20;
            oled.drawRect(x, 26, 16, 36, SSD1306_WHITE);
            int fillH = (int)((digits[i] / 9.0f) * 32.0f);
            if (fillH > 0) {
                oled.fillRect(x + 2, 28 + (32 - fillH), 12, fillH, SSD1306_WHITE);
            }
        }
    }

    // --- OLED Clock Style 6: Cyberpunk Boxed Frame Clock ---
    void drawOledClock_CyberpunkBox(const String& timeStr, const SensorData& s) {
        oled.drawRect(0, 0, 128, 64, SSD1306_WHITE);
        oled.drawRect(2, 2, 124, 60, SSD1306_WHITE);
        oled.fillRect(8, 0, 48, 5, SSD1306_WHITE);
        oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(10, 0);
        oled.print("CYBER");
        
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(3);
        oled.setCursor(14, 16);
        oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : "00:00");
        
        int sec = timeStr.length() >= 8 ? timeStr.substring(6, 8).toInt() : 0;
        oled.setTextSize(1);
        oled.setCursor(102, 18);
        oled.printf("%02d", sec);
        
        oled.drawFastHLine(8, 44, 112, SSD1306_WHITE);
        oled.setCursor(12, 48);
        oled.printf("TEMP: %.1fC  ALT:%dm", s.tempC, (int)s.altitudeM);
    }

    // --- OLED Clock Style 7: Radial Horizon Arc Clock ---
    void drawOledClock_RadialHorizon(const String& timeStr, const SensorData& s) {
        // Horizon curve emulation at bottom
        oled.drawCircle(64, 88, 60, SSD1306_WHITE);
        oled.drawCircle(64, 88, 62, SSD1306_WHITE);
        
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(10, 4);
        oled.print("=== RADIAL HORIZON ===");
        
        oled.setTextSize(3);
        oled.setCursor(20, 20);
        oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : "00:00");
        
        oled.setTextSize(1);
        oled.setCursor(14, 50);
        oled.printf("HUM:%.0f%% | %.1fhPa", s.humidity, s.pressureHpa);
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

        scrollX -= 8;
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
        // Mode 4: Retro Cyber Radar Graphic
        oled.drawCircle(64, 32, 28, SSD1306_WHITE);
        oled.drawCircle(64, 32, 18, SSD1306_WHITE);
        oled.drawCircle(64, 32, 8, SSD1306_WHITE);
        oled.drawLine(36, 32, 92, 32, SSD1306_WHITE);
        oled.drawLine(64, 4, 64, 60, SSD1306_WHITE);

        // Blinking / Rotating Target
        int sec = millis() / 500;
        int tx = 64 + (int)(cos(sec * 0.7f) * 20);
        int ty = 32 + (int)(sin(sec * 0.7f) * 20);
        oled.fillCircle(tx, ty, 3, SSD1306_WHITE);

        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(4, 4);
        oled.print("RADAR");
        oled.setCursor(96, 4);
        oled.print("ACTIVE");
    }

    void drawMode6_WifiInfo(const String& ipStr, const String& ssid, const String& pass) {
        // Mode 6: WiFi Credentials & IP Broadcast Dashboard
        oled.fillRoundRect(0, 0, 128, 14, 2, SSD1306_WHITE);
        oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(14, 3);
        oled.print("WIFI NETWORK INFO");
        
        oled.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
        oled.setCursor(2, 18);
        oled.printf("IP  : %s", ipStr.c_str());
        oled.setCursor(2, 30);
        oled.printf("SSID: %s", ssid.c_str());
        oled.setCursor(2, 42);
        oled.printf("PASS: %s", pass.c_str());
        
        oled.drawRect(0, 53, 128, 11, SSD1306_WHITE);
        oled.setCursor(14, 55);
        oled.print("SCAN TFT FOR QR URL");
    }
};

#endif // DISPLAY_OLED_H
