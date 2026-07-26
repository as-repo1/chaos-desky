#ifndef WATCHFACE_ENGINE_H
#define WATCHFACE_ENGINE_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <math.h>
#include "weather_api.h"

enum WatchFaceStyle {
    WATCHFACE_SWISS_ANALOG = 0,
    WATCHFACE_CYBER_CHRONO = 1,
    WATCHFACE_MODERN_DIGITAL = 2,
    WATCHFACE_NEON_NIXIE = 3
};

class WatchFaceEngine {
public:
    int activeStyle = WATCHFACE_SWISS_ANALOG;

    void render(Adafruit_GFX& gfx, 
                int hours, int mins, int secs, 
                const OutdoorWeatherData& weather,
                float tempC,
                uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg) {
        
        switch (activeStyle) {
            case WATCHFACE_SWISS_ANALOG:
                renderSwissAnalog(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg);
                break;
            case WATCHFACE_CYBER_CHRONO:
                renderCyberChrono(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg);
                break;
            case WATCHFACE_MODERN_DIGITAL:
                renderModernDigital(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg);
                break;
            case WATCHFACE_NEON_NIXIE:
                renderNeonNixie(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg);
                break;
            default:
                renderSwissAnalog(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg);
                break;
        }
    }

private:
    // --- Style 0: Classic Swiss Luxury Analog Dial ---
    void renderSwissAnalog(Adafruit_GFX& gfx, int h, int m, int s, 
                           const OutdoorWeatherData& w, float tempC,
                           uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg) {
        
        int cx = 64;
        int cy = 76;
        int r = 48;

        // Outer Bezel
        gfx.drawCircle(cx, cy, r, colorPrimary);
        gfx.drawCircle(cx, cy, r - 1, colorPrimary);

        // 12 Hour Ticks
        for (int i = 0; i < 12; i++) {
            float angle = i * (2.0f * M_PI / 12.0f);
            int x1 = cx + (int)(cos(angle) * (r - 6));
            int y1 = cy + (int)(sin(angle) * (r - 6));
            int x2 = cx + (int)(cos(angle) * (r - 2));
            int y2 = cy + (int)(sin(angle) * (r - 2));
            gfx.drawLine(x1, y1, x2, y2, colorAccent);
        }

        // Hour Hand
        float angleH = ((h % 12) + m / 60.0f) * (2.0f * M_PI / 12.0f) - (M_PI / 2.0f);
        int hx = cx + (int)(cos(angleH) * (r - 20));
        int hy = cy + (int)(sin(angleH) * (r - 20));
        gfx.drawLine(cx, cy, hx, hy, colorPrimary);
        gfx.drawLine(cx + 1, cy, hx + 1, hy, colorPrimary);

        // Minute Hand
        float angleM = (m + s / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        int mx = cx + (int)(cos(angleM) * (r - 10));
        int my = cy + (int)(sin(angleM) * (r - 10));
        gfx.drawLine(cx, cy, mx, my, colorText);

        // Sweeping Second Hand
        float angleS = s * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        int sx = cx + (int)(cos(angleS) * (r - 6));
        int sy = cy + (int)(sin(angleS) * (r - 6));
        gfx.drawLine(cx, cy, sx, sy, colorAccent);

        // Center Pin
        gfx.fillCircle(cx, cy, 3, colorAccent);

        // Weather Complication Badge below center
        gfx.setTextSize(1);
        gfx.setTextColor(colorText, colorBg);
        gfx.setCursor(24, 134);
        gfx.printf("%.1fC | %s", w.valid ? w.tempC : tempC, w.cityName.substring(0, 8).c_str());
    }

    // --- Style 1: Cyberpunk Dual Chronograph ---
    void renderCyberChrono(Adafruit_GFX& gfx, int h, int m, int s,
                          const OutdoorWeatherData& w, float tempC,
                          uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg) {
        
        // Digital Time Header
        gfx.setTextSize(2);
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setCursor(16, 24);
        gfx.printf("%02d:%02d:%02d", h, m, s);

        // Sub-Dial Mini Analog Clock
        int cx = 64;
        int cy = 82;
        int r = 32;

        gfx.drawCircle(cx, cy, r, colorPrimary);

        float angleM = (m + s / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        int mx = cx + (int)(cos(angleM) * (r - 6));
        int my = cy + (int)(sin(angleM) * (r - 6));
        gfx.drawLine(cx, cy, mx, my, colorText);

        float angleS = s * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        int sx = cx + (int)(cos(angleS) * (r - 4));
        int sy = cy + (int)(sin(angleS) * (r - 4));
        gfx.drawLine(cx, cy, sx, sy, colorAccent);
        gfx.fillCircle(cx, cy, 2, colorAccent);

        // Telemetry Footer
        gfx.setTextSize(1);
        gfx.setTextColor(colorText, colorBg);
        gfx.setCursor(8, 126);
        gfx.printf("INDOOR: %.1f C", tempC);
        gfx.setCursor(8, 138);
        gfx.printf("OUTDOOR:%.1f C", w.tempC);
    }

    // --- Style 2: Modern Minimalist Digital Weather ---
    void renderModernDigital(Adafruit_GFX& gfx, int h, int m, int s,
                            const OutdoorWeatherData& w, float tempC,
                            uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg) {
        
        gfx.fillRoundRect(4, 24, 120, 48, 6, colorPrimary);
        gfx.setTextColor(colorBg, colorPrimary);
        gfx.setTextSize(3);
        gfx.setCursor(10, 36);
        gfx.printf("%02d:%02d", h, m);

        gfx.setTextSize(1);
        gfx.setCursor(102, 36);
        gfx.printf("%02d", s);

        // Weather Stats Box
        gfx.drawRoundRect(4, 78, 120, 64, 4, colorAccent);
        gfx.setTextColor(colorText, colorBg);
        gfx.setCursor(10, 84);
        gfx.printf("CITY: %s", w.cityName.substring(0, 10).c_str());
        gfx.setCursor(10, 98);
        gfx.printf("OUT TEMP: %.1f C", w.tempC);
        gfx.setCursor(10, 112);
        gfx.printf("COND: %s", w.condition.substring(0, 12).c_str());
        gfx.setCursor(10, 126);
        gfx.printf("WIND: %.1f m/s", w.windSpeedMs);
    }

    // --- Style 3: Retro Neon Nixie Tube ---
    void renderNeonNixie(Adafruit_GFX& gfx, int h, int m, int s,
                         const OutdoorWeatherData& w, float tempC,
                         uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg) {
        
        // Digit 1 & 2 (Hours)
        gfx.drawRoundRect(6, 30, 34, 50, 4, colorAccent);
        gfx.drawRoundRect(44, 30, 34, 50, 4, colorAccent);
        gfx.drawRoundRect(84, 30, 38, 50, 4, colorPrimary);

        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(12, 44);
        gfx.printf("%02d", h);
        gfx.setCursor(50, 44);
        gfx.printf("%02d", m);

        gfx.setTextSize(2);
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setCursor(92, 48);
        gfx.printf("%02d", s);

        gfx.setTextSize(1);
        gfx.setCursor(12, 92);
        gfx.print("NIXIE TUBE FACE");
        gfx.setCursor(12, 108);
        gfx.printf("PUNE: %s", w.condition.c_str());
        gfx.setCursor(12, 124);
        gfx.printf("TEMP: %.1f C", w.tempC);
    }
};

#endif // WATCHFACE_ENGINE_H
