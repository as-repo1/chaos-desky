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
    uint8_t enabledPagesMask = 0x1F; // Bitmask for pages 0..4 (default: all enabled)
    int currentRotation = TFT_ROTATION;
    String customBannerText = "ChaosDesky Standalone Station";

    // Color definitions (16-bit RGB565 format)
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
        // Skip disabled pages in mask
        int safety = 0;
        while (((enabledPagesMask & (1 << next)) == 0) && safety < TOTAL_TFT_PAGES) {
            next = (next + 1) % TOTAL_TFT_PAGES;
            safety++;
        }
        currentPage = next;
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
            case 4:
                renderCustomUserPage(fullRedraw);
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
            case 0: tft.print("1/5 OUTDOOR WEATHER"); break;
            case 1: tft.print("2/5 BARO FORECASTER"); break;
            case 2: tft.print("3/5 POMODORO HUB   "); break;
            case 3: tft.print("4/5 SYSTEM & QR    "); break;
            case 4: tft.print("5/5 CUSTOM MEDIA   "); break;
        }
        tft.drawFastHLine(0, 16, 128, COLOR_ACCENT);
    }

    // --- Page 0: Outdoor Weather ---
    void renderOutdoorWeatherPage(const OutdoorWeatherData& w, bool fullRedraw) {
        if (fullRedraw) {
            tft.drawFastHLine(4, 82, 120, COLOR_PRIMARY);
        }

        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(4, 22);
        tft.setTextSize(1);
        tft.printf("LOCATION: %-10s", w.cityName.c_str());

        tft.setTextSize(3);
        tft.setTextColor(COLOR_ACCENT, COLOR_BG);
        tft.setCursor(10, 36);
        if (w.valid) {
            tft.printf("%2.0fC ", w.tempC);
        } else {
            tft.print("--C ");
        }

        if (fullRedraw) {
            drawWeatherIcon(85, 36, w.iconCode);
        }

        tft.setTextSize(1);
        tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
        tft.setCursor(4, 68);
        tft.printf("STATUS: %-12s", w.condition.c_str());

        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(4, 88);
        tft.printf("HUMIDITY: %2.0f%% ", w.humidity);

        tft.setCursor(4, 102);
        tft.printf("TEMP MIN: %4.1fC", w.tempMinC);

        tft.setCursor(4, 116);
        tft.printf("TEMP MAX: %4.1fC", w.tempMaxC);

        tft.setCursor(4, 130);
        tft.printf("WIND:     %4.1fm/s", w.windSpeedMs);

        tft.fillRect(0, 146, 128, 14, COLOR_ACCENT);
        tft.setTextColor(COLOR_BG, COLOR_ACCENT);
        tft.setCursor(4, 149);
        tft.print(w.valid ? "LIVE API SYNC OK   " : "NO API KEY / WIFI  ");
    }

    // --- Page 1: Pressure Graph & Zambretti ---
    void renderPressureGraphPage(SensorManager& sm, bool fullRedraw) {
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setTextSize(1);
        tft.setCursor(4, 20);
        tft.printf("PRESS: %6.1f hPa", sm.data.pressureHpa);

        String forecast = ZambrettiForecaster::calculateForecast(sm.data.pressureHpa, sm.getPastPressure());

        if (fullRedraw) {
            tft.fillRect(4, 32, 120, 20, COLOR_PRIMARY);
            tft.drawRect(4, 58, 120, 62, COLOR_PRIMARY);
        }
        
        tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
        tft.setCursor(8, 38);
        tft.printf("%-20s", forecast.c_str());

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

        tft.fillRect(0, 146, 128, 14, graphColor);
        tft.setTextColor(COLOR_BG, graphColor);
        tft.setCursor(4, 149);
        tft.printf("TREND: %-+5.1fhPa/6h", trendVal);
    }

    // --- Page 2: Pomodoro Hub ---
    void renderPomodoroPage(PomodoroTimer& pomo, const String& timeStr, bool fullRedraw) {
        tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
        tft.setTextSize(2);
        tft.setCursor(18, 22);
        tft.print(timeStr.length() > 5 ? timeStr.substring(0, 5) : timeStr);

        uint16_t stateColor = COLOR_PRIMARY;
        if (pomo.state == POMO_WORK) stateColor = COLOR_ALERT;
        else if (pomo.state == POMO_BREAK) stateColor = COLOR_GOOD;
        else if (pomo.state == POMO_PAUSED) stateColor = COLOR_WARN;

        tft.fillRect(10, 44, 108, 18, stateColor);
        tft.setTextColor(COLOR_BG, stateColor);
        tft.setTextSize(1);
        tft.setCursor(34, 49);
        tft.printf("%-8s", pomo.getStateString().c_str());

        tft.setTextSize(3);
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(16, 68);
        tft.print(pomo.getFormattedTime());

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

        tft.setTextSize(1);
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setCursor(10, 122);
        tft.printf("SESSIONS DONE: %-3d", pomo.completedSessions);

        tft.setCursor(10, 134);
        tft.printf("WORK:%2dm | REST:%2dm", pomo.workDurationMins, pomo.breakDurationMins);

        if (fullRedraw) {
            tft.fillRect(0, 146, 128, 14, COLOR_PRIMARY);
            tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
            tft.setCursor(4, 149);
            tft.print("CONTROL VIA WEB UI ");
        }
    }

    // --- Page 3: System QR ---
    void renderSystemQrPage(const String& ipStr, bool fullRedraw) {
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setTextSize(1);
        tft.setCursor(4, 20);
        tft.printf("IP: %-15s", ipStr.c_str());

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

        tft.setCursor(4, 128);
        tft.printf("RAM FREE: %5u KB", ESP.getFreeHeap() / 1024);

        if (fullRedraw) {
            tft.fillRect(0, 146, 128, 14, COLOR_ACCENT);
            tft.setTextColor(COLOR_BG, COLOR_ACCENT);
            tft.setCursor(4, 149);
            tft.print("SCAN QR FOR WEB DASH");
        }
    }

    // --- Page 4: Custom Uploaded User Image / Banner Page ---
    void renderCustomUserPage(bool fullRedraw) {
        if (fullRedraw) {
            // Check if uploaded custom TFT raw image exists (128x144 pixels 16-bit RGB565)
            if (LittleFS.exists("/custom_tft.raw")) {
                File f = LittleFS.open("/custom_tft.raw", "r");
                if (f && f.size() >= 128 * 128 * 2) {
                    uint16_t lineBuf[128];
                    for (int y = 18; y < 146 && f.available(); y++) {
                        f.read((uint8_t*)lineBuf, 128 * 2);
                        for (int x = 0; x < 128; x++) {
                            tft.drawPixel(x, y, lineBuf[x]);
                        }
                    }
                    f.close();
                    
                    tft.fillRect(0, 146, 128, 14, COLOR_PRIMARY);
                    tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
                    tft.setCursor(4, 149);
                    tft.print("CUSTOM USER IMAGE  ");
                    return;
                }
            }

            // Fallback Custom Text Banner Display
            tft.fillRect(4, 25, 120, 115, COLOR_PRIMARY);
            tft.drawRect(4, 25, 120, 115, COLOR_ACCENT);
            
            tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
            tft.setTextSize(2);
            tft.setCursor(14, 40);
            tft.print("CHAOS");
            tft.setCursor(14, 60);
            tft.print("DESKY");

            tft.setTextSize(1);
            tft.setCursor(10, 90);
            tft.print(customBannerText.substring(0, 18));
            tft.setCursor(10, 105);
            if (customBannerText.length() > 18) {
                tft.print(customBannerText.substring(18, 36));
            }

            tft.fillRect(0, 146, 128, 14, COLOR_ACCENT);
            tft.setTextColor(COLOR_BG, COLOR_ACCENT);
            tft.setCursor(4, 149);
            tft.print("UPLOAD VIA WEB UI  ");
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
