#ifndef DISPLAY_TFT_H
#define DISPLAY_TFT_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <LittleFS.h>
#include <qrcode.h>
#include "config.h"
#include "sensors.h"
#include "weather_api.h"
#include "zambretti.h"
#include "pomodoro.h"
#include "notification_manager.h"
#include "gfx_icons.h"
#include "watchface_engine.h"

extern WatchFaceEngine watchFaceEngine;

enum TFTTheme {
    THEME_CYBERPUNK = 0,
    THEME_MATRIX,
    THEME_DARK_GLASS,
    THEME_RETRO,
    THEME_DRACULA,
    THEME_NORD,
    THEME_GRUVBOX,
    THEME_MONOCHROME,
    THEME_NOTHING_UI,
    THEME_ONE_UI,
    THEME_MATERIAL_YOU
};

class TftDisplayManager {
public:
    int currentPage = 0;
    TFTTheme activeTheme = THEME_CYBERPUNK;
    uint16_t enabledPagesMask = 0x03FF; // Bitmask for pages 0..9 (default: all 10 enabled)
    int currentRotation = TFT_ROTATION;
    String customBannerText = "ChaosDesky Standalone Station";
    
    // Climate Page State
    int activeClimateTheme = 0;
    int activeGraphType = 0; // 0=Pressure, 1=Temp, 2=Humidity

    // Interactive To-Do List State (Context-Aware Controls)
    int todoSelectedIdx = 0; // Currently focused task index (0 to 3)
    bool todoChecked[4] = { true, false, false, false };
    String todoTitles[4] = { "Ship Desky", "Deep Pomo", "Hydration", "Telemetry" };

    // Color definitions (16-bit RGB565 format)
    uint16_t COLOR_BG       = ST77XX_BLACK;
    uint16_t COLOR_PRIMARY  = ST77XX_CYAN;
    uint16_t COLOR_ACCENT   = ST77XX_MAGENTA;
    uint16_t COLOR_TEXT     = ST77XX_WHITE;
    uint16_t COLOR_GOOD     = ST77XX_GREEN;
    uint16_t COLOR_WARN     = ST77XX_YELLOW;
    uint16_t COLOR_ALERT    = ST77XX_RED;

    TftDisplayManager() : tft(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN) {}

    bool begin() {
        SPI.begin(TFT_SCLK_PIN, -1, TFT_MOSI_PIN, TFT_CS_PIN);
        tft.initR(TFT_INIT_TAB);
        tft.setRotation(currentRotation);
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

    void setRotation(int rot) {
        if (rot >= 0 && rot <= 3) {
            currentRotation = rot;
            tft.setRotation(currentRotation);
            pageNeedsFullRedraw = true;
        }
    }

    void applyTheme(TFTTheme theme) {
        activeTheme = theme;
        switch (theme) {
            case THEME_CYBERPUNK:
                COLOR_BG      = ST77XX_BLACK;
                COLOR_PRIMARY = ST77XX_CYAN;
                COLOR_ACCENT  = ST77XX_MAGENTA;
                COLOR_TEXT    = ST77XX_WHITE;
                COLOR_GOOD    = 0x07E0;
                COLOR_WARN    = 0xFFE0;
                COLOR_ALERT   = 0xF800;
                break;
            case THEME_MATRIX:
                COLOR_BG      = ST77XX_BLACK;
                COLOR_PRIMARY = 0x07E0;
                COLOR_ACCENT  = 0x03E0;
                COLOR_TEXT    = 0xAFE0;
                COLOR_GOOD    = 0x07E0;
                COLOR_WARN    = 0xFFE0;
                COLOR_ALERT   = 0xF800;
                break;
            case THEME_DARK_GLASS:
                COLOR_BG      = 0x18E3;
                COLOR_PRIMARY = 0x545F;
                COLOR_ACCENT  = 0xFD20;
                COLOR_TEXT    = ST77XX_WHITE;
                COLOR_GOOD    = 0x3666;
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
            case THEME_DRACULA:
                COLOR_BG      = 0x294D; // #282A36
                COLOR_PRIMARY = 0x877E; // #8BE9FD (Cyan)
                COLOR_ACCENT  = 0xBD59; // #BD93F9 (Purple)
                COLOR_TEXT    = 0xF7BE; // #F8F8F2
                COLOR_GOOD    = 0x5771; // #50FA7B
                COLOR_WARN    = 0xF72B; // #F1FA8C
                COLOR_ALERT   = 0xFB56; // #FF79C6
                break;
            case THEME_NORD:
                COLOR_BG      = 0x2D6B; // #2E3440
                COLOR_PRIMARY = 0x8E1A; // #88C0D0 (Frost)
                COLOR_ACCENT  = 0x8518; // #81A1C1
                COLOR_TEXT    = 0xE71C; // #ECEFF4
                COLOR_GOOD    = 0xA6F4; // #A3BE8C
                COLOR_WARN    = 0xEB0C; // #EBCB8B
                COLOR_ALERT   = 0xBF0B; // #BF616A
                break;
            case THEME_GRUVBOX:
                COLOR_BG      = 0x2945; // #282828
                COLOR_PRIMARY = 0x8E18; // #8EC07C (Aqua)
                COLOR_ACCENT  = 0xFDF0; // #FABD2F (Amber)
                COLOR_TEXT    = 0xEB54; // #EBDBB2
                COLOR_GOOD    = 0xB5A7; // #B8BB26
                COLOR_WARN    = 0xFE27; // #FE8019
                COLOR_ALERT   = 0xFB49; // #FB4934
                break;
            case THEME_MONOCHROME:
                COLOR_BG      = ST77XX_BLACK;
                COLOR_PRIMARY = ST77XX_WHITE;
                COLOR_ACCENT  = ST77XX_WHITE;
                COLOR_TEXT    = ST77XX_WHITE;
                COLOR_GOOD    = ST77XX_WHITE;
                COLOR_WARN    = ST77XX_WHITE;
                COLOR_ALERT   = ST77XX_WHITE;
                break;
            case THEME_NOTHING_UI:
                COLOR_BG      = 0x10A2; // Dark Slate #111
                COLOR_PRIMARY = ST77XX_WHITE;
                COLOR_ACCENT  = ST77XX_RED;   // Nothing Red
                COLOR_TEXT    = ST77XX_WHITE;
                COLOR_GOOD    = ST77XX_WHITE;
                COLOR_WARN    = ST77XX_RED;
                COLOR_ALERT   = ST77XX_RED;
                break;
            case THEME_ONE_UI:
                COLOR_BG      = 0x10E4; // #10141D
                COLOR_PRIMARY = 0x041F; // #0381FE (OneUI Blue)
                COLOR_ACCENT  = 0x555F; // #50A8FE
                COLOR_TEXT    = ST77XX_WHITE;
                COLOR_GOOD    = 0x2E6C; // #28C76F
                COLOR_WARN    = 0xFD20; // #FF9F43
                COLOR_ALERT   = 0xEA4B; // #EA5455
                break;
            case THEME_MATERIAL_YOU:
                COLOR_BG      = 0x18F4; // #1C1B1F
                COLOR_PRIMARY = 0xD5FF; // #D0BCFF (Material Purple)
                COLOR_ACCENT  = 0xCD7B; // #CCC2DC
                COLOR_TEXT    = 0xE71C; // #E6E1E5
                COLOR_GOOD    = 0xB675; // #B6F2BA
                COLOR_WARN    = 0xFFB4; // #FFDCC2
                COLOR_ALERT   = 0xF372; // #F2B8B5
                break;
        }
        pageNeedsFullRedraw = true;
    }

    void nextPage() {
        int next = (currentPage + 1) % TOTAL_TFT_PAGES;
        // Skip disabled pages in bitmask
        int safety = 0;
        while (((enabledPagesMask & (1 << next)) == 0) && safety < TOTAL_TFT_PAGES) {
            next = (next + 1) % TOTAL_TFT_PAGES;
            safety++;
        }
        currentPage = next;
        pageNeedsFullRedraw = true;
    }

    void prevPage() {
        int prev = (currentPage - 1 + TOTAL_TFT_PAGES) % TOTAL_TFT_PAGES;
        int safety = 0;
        while (((enabledPagesMask & (1 << prev)) == 0) && safety < TOTAL_TFT_PAGES) {
            prev = (prev - 1 + TOTAL_TFT_PAGES) % TOTAL_TFT_PAGES;
            safety++;
        }
        currentPage = prev;
        pageNeedsFullRedraw = true;
    }

    void setPage(int page) {
        if (page >= 0 && page < TOTAL_TFT_PAGES) {
            currentPage = page;
            pageNeedsFullRedraw = true;
        }
    }

    void forceRedraw() {
        pageNeedsFullRedraw = true;
    }

    void renderCurrentPage(const OutdoorWeatherData& weather, 
                           SensorManager& sensors, 
                           PomodoroTimer& pomo, 
                           const NotificationManager& notifMgr,
                           const String& ipStr, 
                           const String& timeStr,
                           int oledMode = 0,
                           int oledClockStyle = 0) {
        
        if (notifMgr.isTftActive()) {
            renderNotificationOverlay(notifMgr.currentNotif, notifMgr.getProgress());
            pageNeedsFullRedraw = true; // Request full redraw when notification disappears
            return;
        }

        bool fullRedraw = pageNeedsFullRedraw || (currentPage != lastRenderedPage);

        if (fullRedraw) {
            tft.fillScreen(COLOR_BG);
            drawPageHeader(timeStr);
            drawPageDots();
            lastRenderedPage = currentPage;
            pageNeedsFullRedraw = false;
        }

        switch (currentPage) {
            case 0: renderOutdoorWeatherPage(weather, fullRedraw); break;
            case 1: renderPressureGraphPage(sensors, fullRedraw); break;
            case 2: renderPomodoroPage(pomo, timeStr, fullRedraw); break;
            case 3: renderSystemQrPage(ipStr, fullRedraw); break;
            case 4: renderCustomUserPage(fullRedraw); break;
            case 5: renderIndoorClimatePage(sensors.data, fullRedraw); break;
            case 6: renderBigClockPage(timeStr, weather, sensors, fullRedraw); break;
            case 7: renderNetworkMonitorPage(ipStr, fullRedraw); break;
            case 8: renderSystemHardwarePage(fullRedraw); break;
            case 9: renderOledControlPage(fullRedraw, oledMode, oledClockStyle); break;
        }
    }

private:
    Adafruit_ST7735 tft;
    int lastRenderedPage = -1;
    bool pageNeedsFullRedraw = true;

    void renderNotificationOverlay(const NotificationItem& n, float progress) {
        uint16_t headerColor = COLOR_PRIMARY;
        if (n.category == NOTIF_WARNING) headerColor = COLOR_WARN;
        else if (n.category == NOTIF_ALERT || n.category == NOTIF_CALL) headerColor = COLOR_ALERT;

        tft.fillRoundRect(4, 20, 120, 120, 6, COLOR_BG);
        tft.drawRoundRect(4, 20, 120, 120, 6, headerColor);

        tft.fillRoundRect(4, 20, 120, 20, 6, headerColor);
        tft.setTextColor(COLOR_BG, headerColor);
        tft.setTextSize(1);

        switch (n.category) {
            case NOTIF_INFO:
                tft.setCursor(24, 26);
                tft.print("INFO ALERT");
                break;
            case NOTIF_MESSAGE:
                GfxIconRenderer::drawChatBubble(tft, 8, 23, COLOR_BG);
                tft.setCursor(26, 26);
                tft.print("NEW MESSAGE");
                break;
            case NOTIF_CALL:
                GfxIconRenderer::drawPhone(tft, 8, 22, COLOR_BG);
                tft.setCursor(26, 26);
                tft.print("INCOMING CALL");
                break;
            case NOTIF_WARNING:
                GfxIconRenderer::drawWarningTriangle(tft, 6, 22, COLOR_BG, headerColor);
                tft.setCursor(26, 26);
                tft.print("WARNING!");
                break;
            case NOTIF_ALERT:
                GfxIconRenderer::drawWarningTriangle(tft, 6, 22, COLOR_BG, headerColor);
                tft.setCursor(26, 26);
                tft.print("CRITICAL ALERT");
                break;
        }

        tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
        tft.setTextSize(1);
        tft.setCursor(10, 46);
        tft.print(n.title.substring(0, 18));

        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(10, 62);
        tft.print(n.message.substring(0, 18));

        if (n.message.length() > 18) {
            tft.setCursor(10, 77);
            tft.print(n.message.substring(18, 36));
        }

        if (n.message.length() > 36) {
            tft.setCursor(10, 92);
            tft.print(n.message.substring(36, 54));
        }

        // Rounded Progress bar
        tft.drawRoundRect(8, 120, 112, 10, 3, headerColor);
        int fillWidth = (int)((1.0f - progress) * 108.0f);
        if (fillWidth > 0) {
            tft.fillRoundRect(10, 122, fillWidth, 6, 2, headerColor);
        }

        tft.fillRoundRect(4, 134, 120, 12, 3, headerColor);
        tft.setTextColor(COLOR_BG, headerColor);
        tft.setCursor(10, 136);
        tft.print("DISMISSING NOTIF...");
    }

    void drawPageHeader(const String& timeStr) {
        tft.fillRect(0, 0, 128, 14, COLOR_BG);
        tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
        tft.setTextSize(1);
        tft.setCursor(4, 2);
        tft.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : "00:00");

        // Center Title
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(44, 2);
        switch (currentPage) {
            case 0: tft.print("WEATHER"); break;
            case 1: tft.print("BARO"); break;
            case 2: tft.print("POMODORO"); break;
            case 3: tft.print("SYSTEM"); break;
            case 4: tft.print("TO-DO LIST"); break;
            case 5: tft.print("CLIMATE"); break;
            case 6: tft.print("WATCH"); break;
            case 7: tft.print("NETWORK"); break;
            case 8: tft.print("HARDWARE"); break;
            case 9: tft.print("FLAPPY DESKY"); break;
        }

        GfxIconRenderer::drawWifiSignal(tft, 108, 1, WiFi.RSSI(), COLOR_PRIMARY, COLOR_BG);
        tft.drawFastHLine(0, 14, 128, COLOR_PRIMARY);
    }

    void drawPageDots() {
        tft.fillRect(0, 150, 128, 10, COLOR_BG);
        int startX = (128 - (TOTAL_TFT_PAGES * 8)) / 2;
        for (int i = 0; i < TOTAL_TFT_PAGES; i++) {
            int x = startX + (i * 8) + 3;
            if (i == currentPage) {
                tft.fillCircle(x, 154, 2, COLOR_PRIMARY);
            } else {
                tft.drawCircle(x, 154, 2, COLOR_TEXT);
            }
        }
    }

    // --- Page 0: Outdoor Weather ---
    void renderOutdoorWeatherPage(const OutdoorWeatherData& w, bool fullRedraw) {
        if (fullRedraw) {
            tft.drawRoundRect(4, 16, 120, 52, 6, COLOR_PRIMARY);
            tft.drawRoundRect(4, 72, 120, 74, 6, COLOR_ACCENT);
            drawWeatherIcon(84, 28, w.iconCode);
        }

        // Location & Main Temp Card
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(10, 20);
        tft.setTextSize(1);
        tft.printf("%-10s", w.cityName.substring(0, 10).c_str());

        tft.setTextSize(3);
        tft.setTextColor(COLOR_ACCENT, COLOR_BG);
        tft.setCursor(10, 32);
        if (w.valid) {
            tft.printf("%2.0fC", w.tempC);
        } else {
            tft.print("--C");
        }

        tft.setTextSize(1);
        tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
        tft.setCursor(10, 56);
        tft.printf("%-14s", w.condition.substring(0, 14).c_str());

        // Secondary Metrics Card
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        GfxIconRenderer::drawDroplet(tft, 8, 76, COLOR_PRIMARY);
        tft.setCursor(28, 80);
        tft.printf("HUM: %2.0f%%", w.humidity);

        GfxIconRenderer::drawThermometer(tft, 8, 96, COLOR_TEXT, COLOR_ALERT);
        tft.setCursor(28, 100);
        tft.printf("MIN/MAX: %.0f/%.0fC", w.tempMinC, w.tempMaxC);

        GfxIconRenderer::drawGauge(tft, 8, 116, COLOR_PRIMARY, COLOR_ACCENT);
        tft.setCursor(28, 120);
        tft.printf("WIND: %.1fm/s", w.windSpeedMs);
    }

    // --- Page 1: Pressure/Temp/Humidity Graph ---
    void renderPressureGraphPage(SensorManager& sm, bool fullRedraw) {
        String forecast = ZambrettiForecaster::calculateForecast(sm.data.pressureHpa, sm.getPastPressure());
        float trendVal = sm.data.pressureHpa - sm.getPastPressure();

        if (fullRedraw) {
            tft.fillRoundRect(4, 16, 120, 20, 4, COLOR_PRIMARY);
            tft.drawRoundRect(4, 40, 120, 104, 6, COLOR_PRIMARY);
        }

        // Top Info Pill
        tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
        tft.setTextSize(1);
        tft.setCursor(8, 22);
        
        if (activeGraphType == 0) {
            tft.printf("%-18s", forecast.c_str());
        } else if (activeGraphType == 1) {
            tft.printf("%-18s", "Temperature Trend");
        } else {
            tft.printf("%-18s", "Humidity Trend");
        }

        // Current Value & Trend indicator (if pressure)
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(8, 44);
        
        if (activeGraphType == 0) {
            tft.printf("P: %6.1fhPa", sm.data.pressureHpa);
            GfxIconRenderer::drawTrendArrow(tft, 110, 42, trendVal, trendVal > 0.5f ? COLOR_GOOD : trendVal < -0.5f ? COLOR_ALERT : COLOR_PRIMARY);
        } else if (activeGraphType == 1) {
            tft.printf("T: %6.1fC", sm.data.tempC);
        } else {
            tft.printf("H: %6.1f%%", sm.data.humidity);
        }

        // Graph Plot
        uint16_t graphColor = COLOR_GOOD;
        if (activeGraphType == 0) {
            if (trendVal > 1.5f) graphColor = COLOR_PRIMARY;
            else if (trendVal < -1.5f) graphColor = COLOR_ALERT;
        } else if (activeGraphType == 1) {
            graphColor = COLOR_ALERT; // Red/warm for temp
        } else {
            graphColor = COLOR_PRIMARY; // Cyan/blue for humidity
        }

        if (fullRedraw && sm.historyCount > 1) {
            float minVal = 9999.0f, maxVal = -9999.0f;
            for (int i = 0; i < sm.historyCount; i++) {
                float v = (activeGraphType == 0) ? sm.pressureHistory[i] : (activeGraphType == 1 ? sm.tempHistory[i] : sm.humidityHistory[i]);
                if (v < minVal) minVal = v;
                if (v > maxVal) maxVal = v;
            }
            if (maxVal - minVal < 2.0f) { maxVal += 1.0f; minVal -= 1.0f; }

            int step = 110 / (sm.historyCount - 1);
            for (int i = 0; i < sm.historyCount - 1; i++) {
                float v1 = (activeGraphType == 0) ? sm.pressureHistory[i] : (activeGraphType == 1 ? sm.tempHistory[i] : sm.humidityHistory[i]);
                float v2 = (activeGraphType == 0) ? sm.pressureHistory[i+1] : (activeGraphType == 1 ? sm.tempHistory[i+1] : sm.humidityHistory[i+1]);
                
                int x1 = 8 + (i * step);
                int y1 = 112 - (int)(((v1 - minVal) / (maxVal - minVal)) * 52.0f);
                int x2 = 8 + ((i + 1) * step);
                int y2 = 114 - (int)(((v2 - minVal) / (maxVal - minVal)) * 52.0f);
                tft.drawLine(x1, y1, x2, y2, graphColor);
            }
        }

        // Comfort Indicator Dot
        tft.setCursor(8, 126);
        tft.print("AIR: ");
        if (sm.data.humidity >= 40.0f && sm.data.humidity <= 60.0f) {
            tft.fillCircle(40, 129, 3, COLOR_GOOD);
            tft.setTextColor(COLOR_GOOD, COLOR_BG);
            tft.print(" IDEAL ");
        } else if (sm.data.humidity > 60.0f) {
            tft.fillCircle(40, 129, 3, COLOR_ALERT);
            tft.setTextColor(COLOR_ALERT, COLOR_BG);
            tft.print(" HUMID ");
        } else {
            tft.fillCircle(40, 129, 3, COLOR_WARN);
            tft.setTextColor(COLOR_WARN, COLOR_BG);
            tft.print(" DRY   ");
        }
    }

    // --- Page 2: Pomodoro Hub ---
    void renderPomodoroPage(PomodoroTimer& pomo, const String& timeStr, bool fullRedraw) {
        uint16_t stateColor = COLOR_PRIMARY;
        if (pomo.state == POMO_WORK) stateColor = COLOR_ALERT;
        else if (pomo.state == POMO_BREAK) stateColor = COLOR_GOOD;
        else if (pomo.state == POMO_PAUSED) stateColor = COLOR_WARN;

        if (fullRedraw) {
            tft.drawRoundRect(4, 16, 120, 128, 8, stateColor);
        }

        // State Badge Pill
        tft.fillRoundRect(24, 22, 80, 18, 9, stateColor);
        tft.setTextColor(COLOR_BG, stateColor);
        tft.setTextSize(1);
        tft.setCursor(34, 27);
        tft.printf("%-8s", pomo.getStateString().c_str());

        // Hero Countdown Timer
        tft.setTextSize(3);
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(16, 48);
        tft.print(pomo.getFormattedTime());

        // Progress Arc Bar
        tft.drawRoundRect(12, 80, 104, 12, 4, stateColor);
        int fillWidth = (int)(pomo.getProgress() * 100.0f);
        if (fillWidth > 0) {
            tft.fillRoundRect(14, 82, fillWidth, 8, 3, stateColor);
        }

        // Session Stats
        tft.setTextSize(1);
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(14, 100);
        tft.printf("SESSIONS: %d", pomo.completedSessions);
        tft.setCursor(14, 114);
        tft.printf("WORK:%dm | REST:%dm", pomo.workDurationMins, pomo.breakDurationMins);
        tft.setCursor(14, 128);
        tft.print("CONTROL VIA WEB UI");
    }

    // --- Page 3: System QR ---
    void renderSystemQrPage(const String& ipStr, bool fullRedraw) {
        if (fullRedraw && ipStr.length() > 7 && ipStr != "0.0.0.0") {
            String qrUrl = "http://" + ipStr;
            QRCode qrcode;
            uint8_t qrcodeData[qrcode_getBufferSize(3)];
            qrcode_initText(&qrcode, qrcodeData, 3, 0, qrUrl.c_str());

            int scale = 3;
            int xOffset = (128 - (qrcode.size * scale)) / 2;
            int yOffset = 18;

            tft.fillRoundRect(xOffset - 4, yOffset - 4, (qrcode.size * scale) + 8, (qrcode.size * scale) + 8, 6, ST77XX_WHITE);

            for (uint8_t y = 0; y < qrcode.size; y++) {
                for (uint8_t x = 0; x < qrcode.size; x++) {
                    if (qrcode_getModule(&qrcode, x, y)) {
                        tft.fillRect(xOffset + (x * scale), yOffset + (y * scale), scale, scale, ST77XX_BLACK);
                    }
                }
            }
        }

        // IP Address Badge
        tft.fillRoundRect(8, 108, 112, 16, 4, COLOR_PRIMARY);
        tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
        tft.setTextSize(1);
        tft.setCursor(12, 112);
        tft.printf("IP: %-15s", ipStr.c_str());

        // RAM Line Gauge
        uint32_t freeKb = ESP.getFreeHeap() / 1024;
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(8, 130);
        tft.printf("RAM -f: %u KB", freeKb);
    }

    // --- Page 4: To-Do List & Daily Priorities ---
    void renderCustomUserPage(bool fullRedraw) {
        if (fullRedraw) {
            tft.fillRect(0, 16, 128, 144, COLOR_BG);
            tft.drawRoundRect(2, 18, 124, 140, 6, COLOR_PRIMARY);
            
            tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
            tft.setTextSize(1);
            tft.setCursor(10, 26);
            tft.print("DAILY FOCUS TARGETS:");
            
            tft.drawFastHLine(8, 38, 112, COLOR_PRIMARY);
            
            for (int i = 0; i < 4; i++) {
                int y = 46 + (i * 20);
                if (i == todoSelectedIdx) {
                    tft.fillRoundRect(6, y - 2, 116, 18, 4, COLOR_ACCENT);
                    tft.setTextColor(COLOR_BG, COLOR_ACCENT);
                } else {
                    tft.setTextColor(todoChecked[i] ? COLOR_GOOD : COLOR_TEXT, COLOR_BG);
                }
                tft.setCursor(12, y + 3);
                String prefix = todoChecked[i] ? "[X] " : "[ ] ";
                tft.printf("%s%d. %s", prefix.c_str(), i + 1, todoTitles[i].substring(0, 10).c_str());
            }

            // Banner summary at bottom
            tft.fillRoundRect(6, 130, 116, 22, 4, COLOR_PRIMARY);
            tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
            tft.setCursor(10, 137);
            tft.print("CLICK TO TOGGLE!");
        }
    }

    // --- Page 5: Detailed Indoor Climate Analysis ---
    void renderIndoorClimatePage(const SensorData& s, bool fullRedraw) {
        // Theme Definitions
        uint16_t climatePrimary = COLOR_PRIMARY;
        uint16_t climateAccent = COLOR_ACCENT;
        uint16_t climateAlert = COLOR_ALERT;
        
        if (activeClimateTheme == 0) {
            // Theme 0: Dynamic Thermal
            if (s.tempC < 15.0) { // Cold: Blues/Cyans
                climatePrimary = ST7735_CYAN;
                climateAccent = ST7735_BLUE;
                climateAlert = ST7735_WHITE;
            } else if (s.tempC < 20.0) { // Cool: Greens/Cyans
                climatePrimary = ST7735_GREEN;
                climateAccent = ST7735_CYAN;
                climateAlert = ST7735_BLUE;
            } else if (s.tempC > 30.0) { // Hot: Reds/Oranges
                climatePrimary = ST7735_RED;
                climateAccent = ST7735_ORANGE;
                climateAlert = ST7735_YELLOW;
            } else if (s.tempC > 25.0) { // Warm: Oranges/Yellows
                climatePrimary = ST7735_ORANGE;
                climateAccent = ST7735_YELLOW;
                climateAlert = ST7735_RED;
            }
        } else if (activeClimateTheme == 1) {
            // Theme 1: Cyberpunk
            climatePrimary = ST7735_MAGENTA;
            climateAccent = ST7735_CYAN;
            climateAlert = ST7735_WHITE;
        } else if (activeClimateTheme == 2) {
            // Theme 2: Desert
            climatePrimary = ST7735_ORANGE;
            climateAccent = ST7735_YELLOW;
            climateAlert = ST7735_RED;
        } else if (activeClimateTheme == 3) {
            // Theme 3: Forest
            climatePrimary = ST7735_GREEN;
            climateAccent = tft.color565(173, 255, 47); // GreenYellow
            climateAlert = ST7735_YELLOW;
        } else if (activeClimateTheme == 4) {
            // Theme 4: Neon Ice
            climatePrimary = ST7735_CYAN;
            climateAccent = tft.color565(0, 191, 255); // DeepSkyBlue
            climateAlert = ST7735_WHITE;
        }

        if (activeClimateTheme == 0) {
            // Theme 0: Dynamic Thermal (Standard Dual-Card layout)
            if (fullRedraw) {
                tft.drawRoundRect(4, 16, 120, 42, 6, climatePrimary);
                tft.drawRoundRect(4, 62, 120, 82, 6, climateAccent);
            }

            // Hero Temp & Humidity Header Card
            GfxIconRenderer::drawThermometer(tft, 8, 22, COLOR_TEXT, climateAlert);
            tft.setTextSize(2);
            tft.setTextColor(COLOR_TEXT, COLOR_BG);
            tft.setCursor(20, 24);
            tft.printf("%.1fC", s.tempC);

            GfxIconRenderer::drawDroplet(tft, 76, 22, climatePrimary);
            tft.setCursor(88, 24);
            tft.printf("%.0f%%", s.humidity);

            // 2-Column Sensor Grid Card
            tft.setTextSize(1);
            tft.setTextColor(COLOR_TEXT, COLOR_BG);

            tft.setCursor(10, 68);
            tft.printf("PRESS: %.1fhPa", s.pressureHpa);
            tft.setCursor(10, 82);
            tft.printf("ALT:   %.0fm", s.altitudeM);

            tft.setCursor(10, 98);
            tft.printf("HEAT IND: %.1fC", s.heatIndexC);
            tft.setCursor(10, 112);
            tft.printf("DEW PT:   %.1fC", s.dewPointC);
            tft.setCursor(10, 126);
            tft.printf("MIN/MAX : %.0f/%.0fC", s.minTempC, s.maxTempC);
            
        } else if (activeClimateTheme == 1) {
            // Theme 1: Cyberpunk (Sharp corners, dense tech layout)
            if (fullRedraw) {
                tft.drawRect(2, 16, 124, 128, climateAccent);
                tft.drawLine(2, 44, 126, 44, climatePrimary);
                tft.drawLine(64, 44, 64, 144, climatePrimary);
            }
            tft.setTextSize(1);
            tft.setTextColor(climatePrimary, COLOR_BG);
            tft.setCursor(6, 20); tft.print("TEMP_SYS");
            tft.setCursor(68, 20); tft.print("HUM_SYS");
            
            tft.setTextSize(2);
            tft.setTextColor(COLOR_TEXT, COLOR_BG);
            tft.setCursor(6, 30); tft.printf("%.1f", s.tempC);
            tft.setCursor(68, 30); tft.printf("%.0f", s.humidity);

            tft.setTextSize(1);
            tft.setTextColor(climateAccent, COLOR_BG);
            tft.setCursor(6, 52); tft.print("PRS:"); tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.printf("%.0f", s.pressureHpa);
            tft.setTextColor(climateAccent, COLOR_BG);
            tft.setCursor(6, 72); tft.print("ALT:"); tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.printf("%.0f", s.altitudeM);
            tft.setTextColor(climateAccent, COLOR_BG);
            tft.setCursor(6, 92); tft.print("H_I:"); tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.printf("%.1f", s.heatIndexC);

            tft.setTextColor(climateAccent, COLOR_BG);
            tft.setCursor(68, 52); tft.print("DEW:"); tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.printf("%.1f", s.dewPointC);
            tft.setTextColor(climateAccent, COLOR_BG);
            tft.setCursor(68, 72); tft.print("MIN:"); tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.printf("%.0f", s.minTempC);
            tft.setTextColor(climateAccent, COLOR_BG);
            tft.setCursor(68, 92); tft.print("MAX:"); tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.printf("%.0f", s.maxTempC);
            
        } else {
            // Themes 2, 3, 4: Bubbles / Dashboard layout
            if (fullRedraw) {
                tft.fillCircle(32, 46, 28, climatePrimary);
                tft.fillCircle(96, 46, 28, climateAccent);
                tft.drawRoundRect(4, 80, 120, 64, 8, climatePrimary);
            }
            tft.setTextSize(1);
            tft.setTextColor(COLOR_BG, climatePrimary);
            tft.setCursor(20, 26); tft.print("TMP");
            tft.setTextSize(2);
            tft.setCursor(10, 40); tft.printf("%.1f", s.tempC);

            tft.setTextSize(1);
            tft.setTextColor(COLOR_BG, climateAccent);
            tft.setCursor(84, 26); tft.print("HUM");
            tft.setTextSize(2);
            tft.setCursor(80, 40); tft.printf("%.0f", s.humidity);

            tft.setTextSize(1);
            tft.setTextColor(COLOR_TEXT, COLOR_BG);
            tft.setCursor(10, 88);
            tft.printf("PRESS: %.1fhPa", s.pressureHpa);
            tft.setCursor(10, 102);
            tft.printf("DEW PT: %.1fC", s.dewPointC);
            tft.setCursor(10, 116);
            tft.printf("HEAT I: %.1fC", s.heatIndexC);
            tft.setCursor(10, 130);
            tft.printf("ALT: %.0fm", s.altitudeM);
        }
    }

    // --- Page 6: Custom Watch Face Dial ---
    void renderBigClockPage(const String& timeStr, const OutdoorWeatherData& weather, SensorManager& sensors, bool fullRedraw) {
        int h = 0, m = 0, s = 0;
        if (timeStr.length() >= 8) {
            h = timeStr.substring(0, 2).toInt();
            m = timeStr.substring(3, 5).toInt();
            s = timeStr.substring(6, 8).toInt();
        }

        watchFaceEngine.render(tft, h, m, s, weather, sensors.data.tempC, COLOR_PRIMARY, COLOR_ACCENT, COLOR_TEXT, COLOR_BG, fullRedraw);
    }

    // --- Page 7: Network & Wi-Fi Monitor ---
    void renderNetworkMonitorPage(const String& ipStr, bool fullRedraw) {
        if (fullRedraw) {
            tft.drawRoundRect(4, 16, 120, 128, 8, COLOR_PRIMARY);
        }

        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setTextSize(1);

        tft.setCursor(10, 22);
        tft.printf("SSID: %-11s", WiFi.SSID().substring(0, 11).c_str());
        tft.setCursor(10, 36);
        tft.printf("IP:   %-15s", ipStr.c_str());
        tft.setCursor(10, 50);
        tft.printf("GW:   %-15s", WiFi.gatewayIP().toString().c_str());

        tft.setCursor(10, 68);
        tft.print("MAC ADDR:");
        tft.setCursor(10, 82);
        tft.printf("%-17s", WiFi.macAddress().c_str());

        tft.setCursor(10, 102);
        tft.printf("RSSI: %d dBm", WiFi.RSSI());
        tft.setCursor(10, 118);
        tft.printf("CHANNEL: %d", WiFi.channel());

        GfxIconRenderer::drawWifiSignal(tft, 98, 102, WiFi.RSSI(), COLOR_PRIMARY, COLOR_BG);
    }

    // --- Page 8: System Hardware & Performance ---
    void renderSystemHardwarePage(bool fullRedraw) {
        if (fullRedraw) {
            tft.drawRoundRect(4, 16, 120, 130, 8, COLOR_PRIMARY);
        }
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setTextSize(1);

        tft.setCursor(10, 26);
        tft.printf("FREE HEAP: %u KB", ESP.getFreeHeap() / 1024);
        tft.setCursor(10, 42);
        tft.printf("MIN HEAP:  %u KB", ESP.getMinFreeHeap() / 1024);
        tft.setCursor(10, 58);
        tft.printf("CPU FREQ:  %d MHz", ESP.getCpuFreqMHz());
        tft.setCursor(10, 74);
        tft.printf("CHIP REV:  %d", ESP.getChipRevision());

        tft.setCursor(10, 92);
        tft.printf("FLASH SIZE:%u MB", ESP.getFlashChipSize() / (1024 * 1024));
        tft.setCursor(10, 108);
        tft.printf("UPTIME:    %lu s", millis() / 1000);
        tft.setCursor(10, 124);
        tft.print("BOARD: ESP32-D0WD");
    }

    // --- Page 9 (Index 9, P10): OLED Display Studio & External Controller ---
    void renderOledControlPage(bool fullRedraw, int oledMode, int oledClockStyle) {
        if (fullRedraw) {
            tft.drawRoundRect(4, 14, 120, 134, 6, COLOR_PRIMARY);
            tft.fillRoundRect(8, 18, 112, 18, 4, COLOR_PRIMARY);
            tft.setTextColor(COLOR_BG);
            tft.setTextSize(1);
            tft.setCursor(16, 23);
            tft.print("OLED STUDIO HUB");

            tft.setTextColor(COLOR_ACCENT, COLOR_BG);
            tft.setCursor(12, 44);
            tft.print("[ACTIVE OLED MODE]");

            tft.setTextColor(COLOR_WARN, COLOR_BG);
            tft.setCursor(10, 102);
            tft.print("[RIGHT SWITCH ACTION]");
            tft.setTextColor(COLOR_TEXT, COLOR_BG);
            tft.setCursor(10, 115);
            tft.print("Single: Cycle Mode");
            tft.setCursor(10, 128);
            tft.print("Double: Cycle Clock");
            tft.setCursor(10, 139);
            tft.print("Long  : Screensavers");
        }

        // Always redraw dynamic active mode and clock style info
        tft.fillRect(10, 58, 108, 38, COLOR_BG);
        tft.drawRect(10, 58, 108, 38, COLOR_TEXT);

        tft.setTextColor(ST77XX_WHITE, COLOR_BG);
        tft.setTextSize(1);
        tft.setCursor(16, 64);
        switch (oledMode) {
            case 0: tft.print("0: Telemetry HUD"); break;
            case 1: tft.print("1: Dynamic Clock"); break;
            case 2: tft.print("2: Live Sparklines"); break;
            case 3: tft.print("3: Marquee Ticker"); break;
            case 4: tft.print("4: Screensavers"); break;
            case 5: tft.print("5: WiFi Info"); break;
            default: tft.print("Unknown Mode"); break;
        }

        tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
        tft.setCursor(16, 80);
        if (oledMode == 1) {
            tft.printf("Clock Face: #%d", oledClockStyle + 1);
        } else {
            tft.print("Status: BROADCASTING");
        }
    }

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
