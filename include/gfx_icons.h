#ifndef GFX_ICONS_H
#define GFX_ICONS_H

#include <Arduino.h>
#include <Adafruit_GFX.h>

class GfxIconRenderer {
public:
    // Draw Sun Icon ☀️
    static void drawSun(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t colorCore, uint16_t colorRays) {
        gfx.fillCircle(x + 10, y + 10, 6, colorCore);
        // Sun rays
        gfx.drawFastVLine(x + 10, y + 1, 3, colorRays);
        gfx.drawFastVLine(x + 10, y + 16, 3, colorRays);
        gfx.drawFastHLine(x + 1, y + 10, 3, colorRays);
        gfx.drawFastHLine(x + 16, y + 10, 3, colorRays);
        gfx.drawLine(x + 3, y + 3, x + 5, y + 5, colorRays);
        gfx.drawLine(x + 15, y + 15, x + 17, y + 17, colorRays);
        gfx.drawLine(x + 17, y + 3, x + 15, y + 5, colorRays);
        gfx.drawLine(x + 5, y + 15, x + 3, y + 17, colorRays);
    }

    // Draw Cloud Icon ☁️
    static void drawCloud(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t color) {
        gfx.fillRoundRect(x + 2, y + 8, 18, 8, 3, color);
        gfx.fillCircle(x + 7, y + 7, 5, color);
        gfx.fillCircle(x + 13, y + 6, 6, color);
    }

    // Draw Rain Cloud Icon 🌧️
    static void drawRainCloud(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t colorCloud, uint16_t colorRain) {
        drawCloud(gfx, x, y, colorCloud);
        gfx.drawLine(x + 4, y + 17, x + 2, y + 20, colorRain);
        gfx.drawLine(x + 10, y + 17, x + 8, y + 20, colorRain);
        gfx.drawLine(x + 16, y + 17, x + 14, y + 20, colorRain);
    }

    // Draw Thermometer Icon 🌡️
    static void drawThermometer(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t colorBorder, uint16_t colorMercury) {
        gfx.drawRoundRect(x + 3, y + 1, 6, 12, 2, colorBorder);
        gfx.drawCircle(x + 5, y + 14, 4, colorBorder);
        gfx.fillCircle(x + 5, y + 14, 3, colorMercury);
        gfx.fillRect(x + 4, y + 6, 3, 6, colorMercury);
    }

    // Draw Water Droplet Icon 💧
    static void drawDroplet(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t color) {
        gfx.fillCircle(x + 6, y + 10, 5, color);
        gfx.fillTriangle(x + 6, y + 2, x + 2, y + 10, x + 10, y + 10, color);
    }

    // Draw Pressure Gauge Icon 📈
    static void drawGauge(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t colorArc, uint16_t colorNeedle) {
        gfx.drawCircle(x + 8, y + 8, 8, colorArc);
        gfx.fillCircle(x + 8, y + 8, 2, colorNeedle);
        gfx.drawLine(x + 8, y + 8, x + 13, y + 4, colorNeedle);
    }

    // Draw Stopwatch / Pomodoro Icon ⏱️
    static void drawStopwatch(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t color) {
        gfx.drawCircle(x + 8, y + 10, 7, color);
        gfx.drawFastHLine(x + 6, y + 1, 5, color);
        gfx.drawFastVLine(x + 8, y + 1, 2, color);
        gfx.drawLine(x + 8, y + 10, x + 8, y + 5, color);
        gfx.drawLine(x + 8, y + 10, x + 12, y + 10, color);
    }

    // Draw Wi-Fi Signal Ladder Icon 📶
    static void drawWifiSignal(Adafruit_GFX& gfx, int16_t x, int16_t y, int rssi, uint16_t colorOn, uint16_t colorOff) {
        int bars = 0;
        if (rssi > -60) bars = 4;
        else if (rssi > -70) bars = 3;
        else if (rssi > -80) bars = 2;
        else if (rssi > -90) bars = 1;

        for (int i = 0; i < 4; i++) {
            uint16_t c = (i < bars) ? colorOn : colorOff;
            int h = (i + 1) * 3;
            gfx.fillRect(x + (i * 4), y + 12 - h, 3, h, c);
        }
    }

    // Draw BLE Radar Pulse Icon 📡
    static void drawBleRadar(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t color) {
        gfx.fillCircle(x + 8, y + 12, 2, color);
        gfx.drawCircle(x + 8, y + 12, 5, color);
        gfx.drawCircle(x + 8, y + 12, 8, color);
        gfx.drawFastVLine(x + 8, y + 2, 3, color);
    }

    // Draw Microchip CPU Icon ⚡
    static void drawCpuChip(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t colorBody, uint16_t colorPins) {
        gfx.fillRect(x + 4, y + 4, 10, 10, colorBody);
        // Chip pins
        gfx.drawFastHLine(x + 1, y + 6, 3, colorPins);
        gfx.drawFastHLine(x + 1, y + 11, 3, colorPins);
        gfx.drawFastHLine(x + 14, y + 6, 3, colorPins);
        gfx.drawFastHLine(x + 14, y + 11, 3, colorPins);
        gfx.drawFastVLine(x + 6, y + 1, 3, colorPins);
        gfx.drawFastVLine(x + 11, y + 1, 3, colorPins);
        gfx.drawFastVLine(x + 6, y + 14, 3, colorPins);
        gfx.drawFastVLine(x + 11, y + 14, 3, colorPins);
    }

    // Draw Phone Handset Icon 📞
    static void drawPhone(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t color) {
        gfx.fillCircle(x + 4, y + 4, 2, color);
        gfx.fillCircle(x + 12, y + 12, 2, color);
        gfx.drawLine(x + 4, y + 4, x + 12, y + 12, color);
        gfx.drawLine(x + 5, y + 3, x + 13, y + 11, color);
    }

    // Draw Chat Message Bubble 💬
    static void drawChatBubble(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t color) {
        gfx.fillRoundRect(x + 1, y + 1, 14, 10, 3, color);
        gfx.fillTriangle(x + 3, y + 10, x + 7, y + 10, x + 2, y + 14, color);
    }

    // Draw Warning Triangle ⚠️
    static void drawWarningTriangle(Adafruit_GFX& gfx, int16_t x, int16_t y, uint16_t colorBg, uint16_t colorFg) {
        gfx.fillTriangle(x + 8, y + 1, x + 1, y + 14, x + 15, y + 14, colorBg);
        gfx.drawTriangle(x + 8, y + 1, x + 1, y + 14, x + 15, y + 14, colorFg);
        gfx.drawFastVLine(x + 8, y + 5, 5, colorFg);
        gfx.drawPixel(x + 8, y + 12, colorFg);
    }

    // Draw Trend Arrow (Up/Down/Flat) 📈
    static void drawTrendArrow(Adafruit_GFX& gfx, int16_t x, int16_t y, float trend, uint16_t color) {
        if (trend > 0.5f) {
            // Rising ▲
            gfx.fillTriangle(x + 4, y + 1, x + 1, y + 7, x + 7, y + 7, color);
            gfx.fillRect(x + 3, y + 7, 3, 5, color);
        } else if (trend < -0.5f) {
            // Falling ▼
            gfx.fillRect(x + 3, y + 1, 3, 5, color);
            gfx.fillTriangle(x + 4, y + 11, x + 1, y + 5, x + 7, y + 5, color);
        } else {
            // Stable ═
            gfx.fillRect(x + 1, y + 4, 7, 3, color);
        }
    }
};

#endif // GFX_ICONS_H
