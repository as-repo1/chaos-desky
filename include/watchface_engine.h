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
    WATCHFACE_NEON_NIXIE = 3,
    WATCHFACE_CASIO_F91W = 4,
    WATCHFACE_CASIO_GSHOCK = 5
};

class WatchFaceEngine {
public:
    int activeStyle = WATCHFACE_CASIO_F91W; // Default to iconic Casio F-91W!
    int lastH = -1;
    int lastM = -1;
    int lastS = -1;
    int lastStyle = -1;

    void render(Adafruit_GFX& gfx, 
                int hours, int mins, int secs, 
                const OutdoorWeatherData& weather,
                float tempC,
                uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg,
                bool fullRedraw = false) {
        
        // Detect style change or explicit page transition
        if (activeStyle != lastStyle || fullRedraw) {
            fullRedraw = true;
            lastStyle = activeStyle;
            lastH = -1; lastM = -1; lastS = -1;
        }

        // Zero Flicker Guard: If time hasn't ticked and no full redraw needed, return early
        if (!fullRedraw && hours == lastH && mins == lastM && secs == lastS) {
            return;
        }

        switch (activeStyle) {
            case WATCHFACE_SWISS_ANALOG:
                renderSwissAnalog(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CYBER_CHRONO:
                renderCyberChrono(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_MODERN_DIGITAL:
                renderModernDigital(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_NEON_NIXIE:
                renderNeonNixie(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CASIO_F91W:
                renderCasioF91W(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CASIO_GSHOCK:
                renderCasioGShock(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            default:
                renderCasioF91W(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
        }

        lastH = hours;
        lastM = mins;
        lastS = secs;
    }

private:
    // --- Style 0: Classic Swiss Luxury Analog Dial (Zero-Flicker Overdraw) ---
    void renderSwissAnalog(Adafruit_GFX& gfx, int h, int m, int s, 
                           const OutdoorWeatherData& w, float tempC,
                           uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        int cx = 64;
        int cy = 72;
        int r = 44;

        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            // Double Outer Bezel
            gfx.drawCircle(cx, cy, r + 2, colorPrimary);
            gfx.drawCircle(cx, cy, r, colorPrimary);

            // 60 Minute Ticks & 12 Hour Ticks
            for (int i = 0; i < 60; i++) {
                float angle = i * (2.0f * M_PI / 60.0f);
                if (i % 5 == 0) {
                    int x1 = cx + (int)(cos(angle) * (r - 7));
                    int y1 = cy + (int)(sin(angle) * (r - 7));
                    int x2 = cx + (int)(cos(angle) * (r - 2));
                    int y2 = cy + (int)(sin(angle) * (r - 2));
                    gfx.drawLine(x1, y1, x2, y2, colorAccent);
                } else {
                    int x = cx + (int)(cos(angle) * (r - 3));
                    int y = cy + (int)(sin(angle) * (r - 3));
                    gfx.drawPixel(x, y, colorPrimary);
                }
            }

            // Cardinal Hour Labels (12, 3, 6, 9)
            gfx.setTextSize(1);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(cx - 5, cy - r + 9); gfx.print("12");
            gfx.setCursor(cx + r - 12, cy - 3); gfx.print("3");
            gfx.setCursor(cx - 3, cy + r - 14); gfx.print("6");
            gfx.setCursor(cx - r + 6, cy - 3); gfx.print("9");

            // Weather Complication Badge below dial
            gfx.drawRoundRect(14, 124, 100, 18, 4, colorAccent);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(22, 129);
            gfx.printf("%.1fC | %s", w.valid ? w.tempC : tempC, w.cityName.substring(0, 8).c_str());
        }

        // Erase Old Hands if valid previous state
        if (lastS >= 0) {
            float oldS = lastS * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
            gfx.drawLine(cx, cy, cx + (int)(cos(oldS) * (r - 6)), cy + (int)(sin(oldS) * (r - 6)), colorBg);
            float oldM = (lastM + lastS / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
            gfx.drawLine(cx, cy, cx + (int)(cos(oldM) * (r - 10)), cy + (int)(sin(oldM) * (r - 10)), colorBg);
            float oldH = ((lastH % 12) + lastM / 60.0f) * (2.0f * M_PI / 12.0f) - (M_PI / 2.0f);
            int ohx = cx + (int)(cos(oldH) * (r - 18));
            int ohy = cy + (int)(sin(oldH) * (r - 18));
            gfx.drawLine(cx, cy, ohx, ohy, colorBg);
            gfx.drawLine(cx + 1, cy, ohx + 1, ohy, colorBg);
            gfx.drawLine(cx - 1, cy, ohx - 1, ohy, colorBg);
        }

        // Hour Hand (Thick)
        float angleH = ((h % 12) + m / 60.0f) * (2.0f * M_PI / 12.0f) - (M_PI / 2.0f);
        int hx = cx + (int)(cos(angleH) * (r - 18));
        int hy = cy + (int)(sin(angleH) * (r - 18));
        gfx.drawLine(cx, cy, hx, hy, colorPrimary);
        gfx.drawLine(cx + 1, cy, hx + 1, hy, colorPrimary);
        gfx.drawLine(cx - 1, cy, hx - 1, hy, colorPrimary);

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
    }

    // --- Style 1: Cyberpunk Dual Chronograph ---
    void renderCyberChrono(Adafruit_GFX& gfx, int h, int m, int s,
                           const OutdoorWeatherData& w, float tempC,
                           uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        int cx = 64;
        int cy = 84;
        int r = 32;

        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            gfx.drawCircle(cx, cy, r, colorPrimary);
            gfx.drawCircle(cx, cy, r - 1, colorPrimary);
            gfx.drawRoundRect(4, 126, 120, 24, 4, colorPrimary);
            gfx.setTextSize(1);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(10, 130);
            gfx.printf("INDOOR:  %.1fC", tempC);
            gfx.setCursor(10, 140);
            gfx.printf("OUTDOOR: %.1fC", w.tempC);
        }

        // Digital Time Header with Blinking Colon (Using background color overdraw!)
        gfx.setTextSize(2);
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setCursor(16, 24);
        if (s % 2 == 0) {
            gfx.printf("%02d:%02d:%02d", h, m, s);
        } else {
            gfx.printf("%02d %02d %02d", h, m, s);
        }

        // Erase sub-dial old hands
        if (lastS >= 0) {
            float oldS = lastS * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
            gfx.drawLine(cx, cy, cx + (int)(cos(oldS) * (r - 4)), cy + (int)(sin(oldS) * (r - 4)), colorBg);
            float oldM = (lastM + lastS / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
            gfx.drawLine(cx, cy, cx + (int)(cos(oldM) * (r - 6)), cy + (int)(sin(oldM) * (r - 6)), colorBg);
        }

        float angleM = (m + s / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleM) * (r - 6)), cy + (int)(sin(angleM) * (r - 6)), colorText);

        float angleS = s * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleS) * (r - 4)), cy + (int)(sin(angleS) * (r - 4)), colorAccent);
        gfx.fillCircle(cx, cy, 2, colorAccent);
    }

    // --- Style 2: Modern Minimalist Digital Weather ---
    void renderModernDigital(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, float tempC,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            gfx.fillRoundRect(4, 24, 120, 48, 8, colorPrimary);
            gfx.drawRoundRect(4, 24, 120, 48, 8, colorAccent);
            
            // Weather Stats Box
            gfx.drawRoundRect(4, 78, 120, 68, 6, colorAccent);
            gfx.setTextColor(colorText, colorBg);
            gfx.setTextSize(1);
            gfx.setCursor(10, 86);
            gfx.printf("CITY: %-10s", w.cityName.substring(0, 10).c_str());
            gfx.setCursor(10, 100);
            gfx.printf("OUT TEMP: %.1fC", w.tempC);
            gfx.setCursor(10, 114);
            gfx.printf("COND: %-10s", w.condition.substring(0, 10).c_str());
            gfx.setCursor(10, 128);
            gfx.printf("WIND: %.1fm/s", w.windSpeedMs);
        }

        // Overdraw Digits cleanly over colorPrimary box
        gfx.setTextColor(colorBg, colorPrimary);
        gfx.setTextSize(3);
        gfx.setCursor(12, 36);
        gfx.printf("%02d:%02d", h, m);

        gfx.setTextSize(2);
        gfx.setCursor(98, 42);
        gfx.printf("%02d", s);
    }

    // --- Style 3: Retro Neon Nixie Tube ---
    void renderNeonNixie(Adafruit_GFX& gfx, int h, int m, int s,
                         const OutdoorWeatherData& w, float tempC,
                         uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            // Faux Glow Outline Boxes
            gfx.drawRoundRect(4, 28, 38, 52, 6, colorPrimary);
            gfx.drawRoundRect(45, 28, 38, 52, 6, colorPrimary);
            gfx.drawRoundRect(86, 28, 38, 52, 6, colorAccent);

            gfx.drawRoundRect(6, 30, 34, 48, 4, colorAccent);
            gfx.drawRoundRect(47, 30, 34, 48, 4, colorAccent);
            gfx.drawRoundRect(88, 30, 34, 48, 4, colorPrimary);

            gfx.setTextSize(1);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(14, 90);
            gfx.print("[ NIXIE TUBE CHOP ]");
            gfx.setCursor(14, 106);
            gfx.printf("CITY: %-10s", w.condition.c_str());
            gfx.setCursor(14, 122);
            gfx.printf("TEMP: %.1f C", w.tempC);
        }

        // Number overdraw inside boxes
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(11, 42);
        gfx.printf("%02d", h);
        gfx.setCursor(52, 42);
        gfx.printf("%02d", m);

        gfx.setTextColor(colorAccent, colorBg);
        gfx.setCursor(93, 42);
        gfx.printf("%02d", s);
    }

    // =========================================================
    // ⌚ NEW: Style 4: Iconic Casio F-91W Digital Chronograph
    // =========================================================
    void renderCasioF91W(Adafruit_GFX& gfx, int h, int m, int s,
                         const OutdoorWeatherData& w, float tempC,
                         uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            
            // Outer Resin Watch Frame
            gfx.drawRoundRect(4, 20, 120, 134, 8, colorPrimary);
            gfx.drawRoundRect(5, 21, 118, 132, 7, colorPrimary);
            
            // Top Iconic Casio Blue/Accent Strip
            gfx.fillRect(8, 26, 112, 14, colorPrimary);
            gfx.setTextColor(colorBg, colorPrimary);
            gfx.setTextSize(1);
            gfx.setCursor(12, 29);
            gfx.print("CASIO");
            gfx.setCursor(82, 29);
            gfx.print("F-91W");
            
            // Red/Accent Chronograph Subtext
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(16, 44);
            gfx.print("ALARM  CHRONOGRAPH");
            
            // LCD Window Box (Deep recessed frame)
            gfx.drawRect(10, 58, 108, 60, colorAccent);
            gfx.drawRect(11, 59, 106, 58, colorPrimary);
            
            // Top LCD Indicators
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(16, 64);
            gfx.print("24H   ALM SIG   DEV");

            // Water Resist emblem at bottom of face
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(22, 124);
            gfx.print("WATER  RESIST");
            gfx.drawFastHLine(22, 134, 84, colorPrimary);
            
            // Telemetry badge under LCD
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(24, 142);
            gfx.printf("TEMP: %.1f C", tempC);
        }

        // Overdraw Big LCD Digits
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(18, 80);
        if (s % 2 == 0) {
            gfx.printf("%02d:%02d", h, m);
        } else {
            gfx.printf("%02d %02d", h, m);
        }

        // Small Trailing Seconds
        gfx.setTextSize(2);
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setCursor(92, 86);
        gfx.printf("%02d", s);
    }

    // =========================================================
    // 🛡️ NEW: Style 5: Casio G-Shock Databank Tactical HUD
    // =========================================================
    void renderCasioGShock(Adafruit_GFX& gfx, int h, int m, int s,
                           const OutdoorWeatherData& w, float tempC,
                           uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);

            // Chamfered / Octagonal G-Shock Frame
            gfx.drawRect(6, 22, 116, 130, colorPrimary);
            gfx.drawLine(6, 22, 16, 16, colorAccent);
            gfx.drawLine(112, 16, 122, 22, colorAccent);
            
            // Bold Headers
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setTextSize(1);
            gfx.setCursor(38, 25);
            gfx.print("G-SHOCK");
            gfx.setCursor(32, 138);
            gfx.print("PROTECTION");
            
            // 3 Tactical Telemetry Dials Across Top
            gfx.drawRect(10, 38, 34, 28, colorPrimary);
            gfx.drawRect(47, 38, 34, 28, colorPrimary);
            gfx.drawRect(84, 38, 34, 28, colorPrimary);

            gfx.setCursor(14, 42); gfx.print("TMP");
            gfx.setCursor(14, 54); gfx.printf("%.0fC", tempC);

            gfx.setCursor(52, 42); gfx.print("OUT");
            gfx.setCursor(52, 54); gfx.printf("%.0fC", w.valid ? w.tempC : 0.0f);

            gfx.setCursor(88, 42); gfx.print("WND");
            gfx.setCursor(88, 54); gfx.printf("%.0fm", w.windSpeedMs);

            // Main Tactical Display Frame
            gfx.drawRoundRect(10, 72, 108, 60, 4, colorAccent);
            gfx.setCursor(16, 77);
            gfx.print("DATABANK   CHRONO");
            
            gfx.drawFastHLine(14, 87, 100, colorPrimary);
        }

        // Main Tactical Time
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(16, 95);
        gfx.printf("%02d:%02d", h, m);

        // Sub Seconds Pulse
        gfx.setTextSize(1);
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setCursor(94, 98);
        gfx.print("SEC");
        gfx.setTextSize(2);
        gfx.setCursor(94, 108);
        gfx.printf("%02d", s);
    }
};

#endif // WATCHFACE_ENGINE_H
