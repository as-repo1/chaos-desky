#ifndef DISPLAY_TFT_H
#define DISPLAY_TFT_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <qrcode.h>
#include "config.h"
#include "sensors.h"
#include "weather_api.h"
#include "zambretti.h"
#include "pomodoro.h"

enum TFTTheme {
    THEME_CYBERPUNK = 0,
    THEME_MATRIX,
    THEME_DARK_GLASS,
    THEME_RETRO
};

class TftDisplayManager {
public:
    int currentPage = 0;
    TFTTheme activeTheme = THEME_CYBERPUNK;

    // Color definitions (16-bit 565 format)
    uint16_t COLOR_BG       = ST77XX_BLACK;
    uint16_t COLOR_PRIMARY  = ST77XX_CYAN;
    uint16_t COLOR_ACCENT   = ST77XX_MAGENTA;
    uint16_t COLOR_TEXT     = ST77XX_WHITE;
    uint16_t COLOR_GOOD     = ST77XX_GREEN;
    uint16_t COLOR_WARN     = ST77XX_YELLOW;
    uint16_t COLOR_ALERT    = ST77XX_RED;

    TftDisplayManager() : tft(TFT_CS_PIN, TFT_DC_PIN, TFT_MOSI_PIN, TFT_SCLK_PIN, TFT_RST_PIN) {}

    bool begin() {
        SPI.begin(TFT_SCLK_PIN, -1, TFT_MOSI_PIN, TFT_CS_PIN);
        tft.initR(TFT_INIT_TAB);
        tft.setRotation(TFT_ROTATION); // 180 degree rotation
        applyTheme(activeTheme);
        
        tft.fillScreen(COLOR_BG);
        tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
        tft.setTextSize(2);
        tft.setCursor(10, 30);
        tft.println("CHAOS");
        tft.setTextColor(COLOR_ACCENT, COLOR_BG);
        tft.setCursor(10, 55);
        tft.println("DESKY");
        tft.setTextSize(1);
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(10, 90);
        tft.println("Initializing TFT...");
        
        pageNeedsFullRedraw = true;
        return true;
    }

    void applyTheme(TFTTheme theme) {
        activeTheme = theme;
        switch (theme) {
            case THEME_CYBERPUNK:
                COLOR_BG      = ST77XX_BLACK;
                COLOR_PRIMARY = ST77XX_CYAN;
                COLOR_ACCENT  = ST77XX_MAGENTA;
                COLOR_TEXT    = ST77XX_WHITE;
                COLOR_GOOD    = 0x07E0; // Neon Green
                COLOR_WARN    = 0xFFE0; // Yellow
                COLOR_ALERT   = 0xF800; // Bright Red
                break;
            case THEME_MATRIX:
                COLOR_BG      = ST77XX_BLACK;
                COLOR_PRIMARY = 0x07E0; // Green
                COLOR_ACCENT  = 0x05E0; // Dark Green
                COLOR_TEXT    = 0xAFE0; // Light Green
                COLOR_GOOD    = 0x07E0;
                COLOR_WARN    = 0xFFE0;
                COLOR_ALERT   = 0xF800;
                break;
            case THEME_DARK_GLASS:
                COLOR_BG      = 0x18E3; // Dark Slate Gray
                COLOR_PRIMARY = 0x545F; // Light Blue
                COLOR_ACCENT  = 0xFD20; // Pastel Orange
                COLOR_TEXT    = ST77XX_WHITE;
                COLOR_GOOD    = 0x3666; // Soft Green
                COLOR_WARN    = 0xFDA0;
                COLOR_ALERT   = 0xFA20;
                break;
            case THEME_RETRO:
                COLOR_BG      = ST77XX_BLACK;
                COLOR_PRIMARY = ST77XX_YELLOW;
                COLOR_ACCENT  = ST77XX_RED;
                COLOR_TEXT    = ST77XX_WHITE;
                COLOR_GOOD    = ST77XX_GREEN;
                COLOR_WARN    = ST77XX_ORANGE;
                COLOR_ALERT   = ST77XX_MAGENTA;
                break;
        }
        pageNeedsFullRedraw = true;
    }

    void nextPage() {
        currentPage = (currentPage + 1) % TOTAL_TFT_PAGES;
        pageNeedsFullRedraw = true;
    }

    void setPage(int page) {
        if (page >= 0 && page < TOTAL_TFT_PAGES && page != currentPage) {
            currentPage = page;
            pageNeedsFullRedraw = true;
        }
    }

    void renderCurrentPage(const OutdoorWeatherData& weather, 
                           SensorManager& sensors, 
                           PomodoroTimer& pomo, 
                           const String& ipStr, 
                           const String& timeStr) {
        
        bool fullRedraw = pageNeedsFullRedraw || (currentPage != lastRenderedPage);

        if (fullRedraw) {
            tft.fillScreen(COLOR_BG);
            drawPageHeader();
            lastRenderedPage = currentPage;
            pageNeedsFullRedraw = false;
        }

        switch (currentPage) {
            case 0:
                renderOutdoorWeatherPage(weather, fullRedraw);
                break;
            case 1:
                renderPressureGraphPage(sensors, fullRedraw);
                break;
            case 2:
                renderPomodoroPage(pomo, timeStr, fullRedraw);
                break;
            case 3:
                renderSystemQrPage(ipStr, fullRedraw);
                break;
        }
    }

private:
    Adafruit_ST7735 tft;
    int lastRenderedPage = -1;
    bool pageNeedsFullRedraw = true;

    void drawPageHeader() {
        tft.fillRect(0, 0, 128, 16, COLOR_PRIMARY);
        tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
        tft.setTextSize(1);
        tft.setCursor(4, 4);

        switch (currentPage) {
            case 0: tft.print("1/4 OUTDOOR WEATHER"); break;
            case 1: tft.print("2/4 BARO FORECASTER"); break;
            case 2: tft.print("3/4 POMODORO HUB   "); break;
            case 3: tft.print("4/4 SYSTEM & QR    "); break;
        }
        tft.drawFastHLine(0, 16, 128, COLOR_ACCENT);
    }

    // --- Page 1: Outdoor Internet Weather ---
    void renderOutdoorWeatherPage(const OutdoorWeatherData& w, bool fullRedraw) {
        if (fullRedraw) {
            tft.drawFastHLine(4, 82, 120, COLOR_PRIMARY);
        }

        // City & Location
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(4, 22);
        tft.setTextSize(1);
        tft.printf("LOCATION: %-10s", w.cityName.c_str());

        // Large Temperature Display
        tft.setTextSize(3);
        tft.setTextColor(COLOR_ACCENT, COLOR_BG);
        tft.setCursor(10, 36);
        if (w.valid) {
            tft.printf("%2.0fC ", w.tempC);
        } else {
            tft.print("--C ");
        }

        // Weather Graphic Icon
        if (fullRedraw) {
            drawWeatherIcon(85, 36, w.iconCode);
        }

        // Condition Text Banner
        tft.setTextSize(1);
        tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
        tft.setCursor(4, 68);
        tft.printf("STATUS: %-12s", w.condition.c_str());

        // Forecast Grid
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(4, 88);
        tft.printf("HUMIDITY: %2.0f%% ", w.humidity);

        tft.setCursor(4, 102);
        tft.printf("TEMP MIN: %4.1fC", w.tempMinC);

        tft.setCursor(4, 116);
        tft.printf("TEMP MAX: %4.1fC", w.tempMaxC);

        tft.setCursor(4, 130);
        tft.printf("WIND:     %4.1fm/s", w.windSpeedMs);

        // Footer Alert Banner
        tft.fillRect(0, 146, 128, 14, COLOR_ACCENT);
        tft.setTextColor(COLOR_BG, COLOR_ACCENT);
        tft.setCursor(4, 149);
        if (w.valid) {
            tft.print("LIVE API SYNC OK   ");
        } else {
            tft.print("NO API KEY / WIFI  ");
        }
    }

    // --- Page 2: Pressure Graph & Zambretti Predictor ---
    void renderPressureGraphPage(SensorManager& sm, bool fullRedraw) {
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setTextSize(1);
        tft.setCursor(4, 20);
        tft.printf("PRESS: %6.1f hPa", sm.data.pressureHpa);

        // Calculate Zambretti Forecast
        String forecast = ZambrettiForecaster::calculateForecast(sm.data.pressureHpa, sm.getPastPressure());

        // Zambretti Banner Box
        if (fullRedraw) {
            tft.fillRect(4, 32, 120, 20, COLOR_PRIMARY);
            tft.drawRect(4, 58, 120, 62, COLOR_PRIMARY);
        }
        
        tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
        tft.setCursor(8, 38);
        tft.printf("%-20s", forecast.c_str());

        // Plot Sparkline Graph
        float trendVal = sm.data.pressureHpa - sm.getPastPressure();
        uint16_t graphColor = COLOR_GOOD;
        if (trendVal > 1.5f) graphColor = COLOR_PRIMARY;
        else if (trendVal < -1.5f) graphColor = COLOR_ALERT;

        if (fullRedraw && sm.historyCount > 1) {
            float minP = 1200.0f, maxP = 0.0f;
            for (int i = 0; i < sm.historyCount; i++) {
                if (sm.pressureHistory[i] < minP) minP = sm.pressureHistory[i];
                if (sm.pressureHistory[i] > maxP) maxP = sm.pressureHistory[i];
            }
            if (maxP - minP < 2.0f) { maxP += 1.0f; minP -= 1.0f; }

            int step = 118 / (sm.historyCount - 1);
            for (int i = 0; i < sm.historyCount - 1; i++) {
                int x1 = 5 + (i * step);
                int y1 = 118 - (int)(((sm.pressureHistory[i] - minP) / (maxP - minP)) * 56.0f);
                int x2 = 5 + ((i + 1) * step);
                int y2 = 118 - (int)(((sm.pressureHistory[i + 1] - minP) / (maxP - minP)) * 56.0f);
                tft.drawLine(x1, y1, x2, y2, graphColor);
            }
        }

        // Comfort Indicator Bar
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(4, 124);
        tft.print("COMFORT: ");
        if (sm.data.humidity >= 40.0f && sm.data.humidity <= 60.0f) {
            tft.setTextColor(COLOR_GOOD, COLOR_BG);
            tft.print("IDEAL (GOOD) ");
        } else if (sm.data.humidity > 60.0f) {
            tft.setTextColor(COLOR_ALERT, COLOR_BG);
            tft.print("HUMID (MOLD) ");
        } else {
            tft.setTextColor(COLOR_WARN, COLOR_BG);
            tft.print("DRY (AIR)   ");
        }

        // Footer Trend
        tft.fillRect(0, 146, 128, 14, graphColor);
        tft.setTextColor(COLOR_BG, graphColor);
        tft.setCursor(4, 149);
        tft.printf("TREND: %-+5.1fhPa/6h", trendVal);
    }

    // --- Page 3: Cyberpunk Pomodoro Hub ---
    void renderPomodoroPage(PomodoroTimer& pomo, const String& timeStr, bool fullRedraw) {
        // Digital Clock Banner
        tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
        tft.setTextSize(2);
        tft.setCursor(18, 22);
        tft.print(timeStr.length() > 5 ? timeStr.substring(0, 5) : timeStr);

        // Pomodoro State Box
        uint16_t stateColor = COLOR_PRIMARY;
        if (pomo.state == POMO_WORK) stateColor = COLOR_ALERT;
        else if (pomo.state == POMO_BREAK) stateColor = COLOR_GOOD;
        else if (pomo.state == POMO_PAUSED) stateColor = COLOR_WARN;

        tft.fillRect(10, 44, 108, 18, stateColor);
        tft.setTextColor(COLOR_BG, stateColor);
        tft.setTextSize(1);
        tft.setCursor(34, 49);
        tft.printf("%-8s", pomo.getStateString().c_str());

        // Big Countdown Timer
        tft.setTextSize(3);
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(16, 68);
        tft.print(pomo.getFormattedTime());

        // Progress Bar
        if (fullRedraw) {
            tft.drawRect(10, 98, 108, 14, COLOR_PRIMARY);
        }
        int fillWidth = (int)(pomo.getProgress() * 104.0f);
        if (fillWidth > 0) {
            tft.fillRect(12, 100, fillWidth, 10, stateColor);
            if (fillWidth < 104) {
                tft.fillRect(12 + fillWidth, 100, 104 - fillWidth, 10, COLOR_BG);
            }
        } else {
            tft.fillRect(12, 100, 104, 10, COLOR_BG);
        }

        // Sessions Completed Counter
        tft.setTextSize(1);
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(10, 122);
        tft.printf("SESSIONS DONE: %-3d", pomo.completedSessions);

        tft.setCursor(10, 134);
        tft.printf("WORK:%2dm | REST:%2dm", pomo.workDurationMins, pomo.breakDurationMins);

        // Footer Action Helper
        if (fullRedraw) {
            tft.fillRect(0, 146, 128, 14, COLOR_PRIMARY);
            tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
            tft.setCursor(4, 149);
            tft.print("CONTROL VIA WEB UI ");
        }
    }

    // --- Page 4: System Info & Dynamic QR Code ---
    void renderSystemQrPage(const String& ipStr, bool fullRedraw) {
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setTextSize(1);
        tft.setCursor(4, 20);
        tft.printf("IP: %-15s", ipStr.c_str());

        // Render QR Code ONCE on fullRedraw
        if (fullRedraw && ipStr.length() > 7 && ipStr != "0.0.0.0") {
            String qrUrl = "http://" + ipStr;
            QRCode qrcode;
            uint8_t qrcodeData[qrcode_getBufferSize(3)];
            qrcode_initText(&qrcode, qrcodeData, 3, 0, qrUrl.c_str());

            int scale = 3;
            int xOffset = (128 - (qrcode.size * scale)) / 2;
            int yOffset = 34;

            tft.fillRect(xOffset - 2, yOffset - 2, (qrcode.size * scale) + 4, (qrcode.size * scale) + 4, ST77XX_WHITE);

            for (uint8_t y = 0; y < qrcode.size; y++) {
                for (uint8_t x = 0; x < qrcode.size; x++) {
                    if (qrcode_getModule(&qrcode, x, y)) {
                        tft.fillRect(xOffset + (x * scale), yOffset + (y * scale), scale, scale, ST77XX_BLACK);
                    }
                }
            }
        }

        // Memory Stats
        tft.setCursor(4, 128);
        tft.printf("RAM FREE: %5u KB", ESP.getFreeHeap() / 1024);

        if (fullRedraw) {
            tft.fillRect(0, 146, 128, 14, COLOR_ACCENT);
            tft.setTextColor(COLOR_BG, COLOR_ACCENT);
            tft.setCursor(4, 149);
            tft.print("SCAN QR FOR WEB DASH");
        }
    }

    // Helper: Draw static weather icon
    void drawWeatherIcon(int x, int y, const String& iconCode) {
        tft.fillRect(x, y, 32, 28, COLOR_BG);
        if (iconCode.indexOf("01") != -1) {
            tft.fillCircle(x + 12, y + 12, 10, ST77XX_YELLOW);
            tft.drawCircle(x + 12, y + 12, 13, ST77XX_ORANGE);
        } else if (iconCode.indexOf("09") != -1 || iconCode.indexOf("10") != -1) {
            tft.fillRoundRect(x + 2, y + 6, 20, 10, 4, ST77XX_WHITE);
            tft.fillCircle(x + 8, y + 6, 6, ST77XX_WHITE);
            tft.drawLine(x + 4, y + 18, x + 2, y + 22, ST77XX_CYAN);
            tft.drawLine(x + 10, y + 18, x + 8, y + 22, ST77XX_CYAN);
            tft.drawLine(x + 16, y + 18, x + 14, y + 22, ST77XX_CYAN);
        } else {
            tft.fillRoundRect(x + 2, y + 8, 20, 12, 5, ST77XX_WHITE);
            tft.fillCircle(x + 8, y + 6, 7, ST77XX_WHITE);
        }
    }
};

#endif // DISPLAY_TFT_H
