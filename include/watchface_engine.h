#ifndef WATCHFACE_ENGINE_H
#define WATCHFACE_ENGINE_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <math.h>
#include "weather_api.h"
#include "sensors.h"
#include "pomodoro.h"

enum WatchFaceStyle {
    WATCHFACE_SWISS_ANALOG = 0,
    WATCHFACE_CYBER_CHRONO = 1,
    WATCHFACE_MODERN_DIGITAL = 2,
    WATCHFACE_NEON_NIXIE = 3,
    WATCHFACE_RETRO_FLIP = 4,
    WATCHFACE_BINARY_MATRIX = 5,
    WATCHFACE_CRT_TERMINAL = 6,
    WATCHFACE_ORBITAL_RINGS = 7,
    WATCHFACE_AVIATION_ALTIMETER = 8,
    WATCHFACE_MINIMALIST_EINK = 9,
    WATCHFACE_STEAMPUNK_GAUGE = 10,
    WATCHFACE_SEGMENT_LED = 11,
    WATCHFACE_CYBERPUNK_2077 = 12,
    WATCHFACE_CHRONOGRAPH = 13,
    WATCHFACE_BAUHAUS = 14
};

class WatchFaceEngine {
public:
    int activeStyle = WATCHFACE_RETRO_FLIP; // Default to iconic Casio F-91W!
    int lastH = -1;
    int lastM = -1;
    int lastS = -1;
    int lastStyle = -1;

    void render(Adafruit_GFX& gfx, 
                int hours, int mins, int secs, 
                const OutdoorWeatherData& weather,
                const SensorManager& sensors, const PomodoroTimer& pomo,
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
                renderSwissAnalog(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CYBER_CHRONO:
                renderCyberChrono(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_MODERN_DIGITAL:
                renderModernDigital(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_NEON_NIXIE:
                renderNeonNixie(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_RETRO_FLIP:
                renderRetroFlip(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_BINARY_MATRIX:
                renderBinaryMatrix(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CRT_TERMINAL:
                renderCRTTerminal(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_ORBITAL_RINGS:
                renderOrbitalRings(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_AVIATION_ALTIMETER:
                renderAviationAltimeter(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_MINIMALIST_EINK:
                renderMinimalistEink(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_STEAMPUNK_GAUGE:
                renderSteampunkGauge(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_SEGMENT_LED:
                renderSegmentLED(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CYBERPUNK_2077:
                renderCyberpunk2077(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CHRONOGRAPH:
                renderChronograph(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_BAUHAUS:
                renderBauhaus(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            default:
                renderRetroFlip(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
        }

        lastH = hours;
        lastM = mins;
        lastS = secs;
    }

private:
    // --- Style 0: Classic Swiss Luxury Analog Dial (Zero-Flicker Overdraw) ---
    void renderSwissAnalog(Adafruit_GFX& gfx, int h, int m, int s, 
                           const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                           uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        int cx = 64, cy = 72, r = 44;

        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            // Sleek Outer Bezel
            gfx.drawCircle(cx, cy, r + 2, colorPrimary);
            
            // Minimalist ticks
            for (int i = 0; i < 60; i++) {
                float angle = i * (2.0f * M_PI / 60.0f);
                if (i % 5 == 0) {
                    gfx.drawLine(cx + (int)(cos(angle) * (r - 6)), cy + (int)(sin(angle) * (r - 6)),
                                 cx + (int)(cos(angle) * (r - 2)), cy + (int)(sin(angle) * (r - 2)), colorAccent);
                } else {
                    gfx.drawPixel(cx + (int)(cos(angle) * (r - 3)), cy + (int)(sin(angle) * (r - 3)), colorPrimary);
                }
            }

            // Trend & Pomo Indicators
            gfx.setTextSize(1);
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(14, 124);
            if ((pomo.state != POMO_IDLE)) gfx.printf("POMO: %s", (pomo.state == POMO_WORK) ? "FOCUS" : "BREAK");
            else gfx.print("POMO: IDLE");

            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(14, 138);
            gfx.printf("PRS: %d hPa", (int)sensors.data.pressureHpa);
        }

        // Fast Clear old hands (Inner circle)
        gfx.fillCircle(cx, cy, r - 7, colorBg);
        
        // Pomo Ring Micro-animation (if active, draw a small arc)
        if ((pomo.state != POMO_IDLE) && pomo.durationSec > 0) {
            float progress = 1.0f - ((float)pomo.remainingSec / pomo.durationSec);
            int pAngle = (int)(progress * 360);
            for(int a=0; a<pAngle; a+=5) {
                float rad = (a - 90) * M_PI / 180.0f;
                gfx.drawPixel(cx + (int)(cos(rad)*15), cy + (int)(sin(rad)*15), colorAccent);
            }
        }

        // Draw Hands
        float angleH = ((h % 12) + m / 60.0f) * (2.0f * M_PI / 12.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleH) * 20), cy + (int)(sin(angleH) * 20), colorPrimary);
        
        float angleM = (m + s / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleM) * 32), cy + (int)(sin(angleM) * 32), colorText);
        
        float angleS = s * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleS) * 38), cy + (int)(sin(angleS) * 38), colorAccent);
        
        gfx.fillCircle(cx, cy, 2, colorPrimary);
    }

    // --- Style 1: Cyberpunk Dual Chronograph ---
    void renderCyberChrono(Adafruit_GFX& gfx, int h, int m, int s,
                           const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                           uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        int cx = 64, cy = 84, r = 32;

        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            gfx.drawCircle(cx, cy, r, colorPrimary);
            gfx.drawRoundRect(4, 126, 120, 24, 4, colorPrimary);
            gfx.setTextSize(1);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(10, 130);
            gfx.printf("IN: %.1f OUT: %.1f", sensors.data.tempC, w.tempC);
            gfx.setCursor(10, 140);
            gfx.printf("POMO: %s", (pomo.state != POMO_IDLE) ? ((pomo.state == POMO_WORK) ? "FOCUS" : "BREAK") : "IDLE");
        }

        // Digital Time Header with Blinking Colon
        gfx.setTextSize(2);
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setCursor(16, 24);
        if (s % 2 == 0) gfx.printf("%02d:%02d:%02d", h, m, s);
        else gfx.printf("%02d %02d %02d", h, m, s);

        // Erase sub-dial interior
        gfx.fillCircle(cx, cy, r - 2, colorBg);

        // Active Pomodoro Circular Ring
        if ((pomo.state != POMO_IDLE) && pomo.durationSec > 0) {
            float progress = 1.0f - ((float)pomo.remainingSec / pomo.durationSec);
            int pAngle = (int)(progress * 360);
            for(int a=0; a<pAngle; a+=2) {
                float rad = (a - 90) * M_PI / 180.0f;
                gfx.drawPixel(cx + (int)(cos(rad)*28), cy + (int)(sin(rad)*28), colorPrimary);
            }
        }

        // Analog Hands
        float angleM = (m + s / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleM) * 22), cy + (int)(sin(angleM) * 22), colorText);

        float angleS = s * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleS) * 28), cy + (int)(sin(angleS) * 28), colorAccent);
        gfx.fillCircle(cx, cy, 2, colorAccent);
    }

    // --- Style 2: Modern Minimalist Digital (Fixed UI & Zero Overlap) ---
    void renderModernDigital(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            
            // Clean top title badge
            gfx.fillRoundRect(6, 20, 116, 18, 4, colorPrimary);
            gfx.setTextColor(colorBg, colorPrimary);
            gfx.setTextSize(1);
            gfx.setCursor(10, 25);
            gfx.print("MODERN DIGITAL HUD");

            // Main digital time container frame
            gfx.drawRoundRect(4, 42, 120, 48, 6, colorAccent);
            gfx.drawRoundRect(5, 43, 118, 46, 5, colorPrimary);
            
            // Environmental Telemetry Box below
            gfx.drawRoundRect(4, 94, 120, 60, 6, colorPrimary);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(10, 102);
            gfx.printf("CITY : %-10s", w.cityName.substring(0, 10).c_str());
            gfx.setCursor(10, 118);
            gfx.printf("TEMP : %.1fC / %.1fC", sensors.data.tempC, w.tempC);
            gfx.setCursor(10, 134);
            gfx.printf("WIND : %.1fm/s %s", w.windSpeedMs, w.condition.substring(0, 4).c_str());
        }

        // Clean time rendering with exact zero-overlap margins
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(8, 54);
        gfx.printf("%02d:%02d", h, m);

        // Separated sleek second pillbox
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(2);
        gfx.setCursor(98, 60);
        gfx.printf("%02d", s);
    }

    // --- Style 3: Retro Neon Nixie Tube (Fixed Geometry & Glass Glow) ---
    void renderNeonNixie(Adafruit_GFX& gfx, int h, int m, int s,
                         const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                         uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            
            // 3 Staggered Nixie Glass Vacuum Tubes (Each 36px wide, zero clash)
            // Tube 1 (Hours): x=4 to x=40
            gfx.drawRoundRect(4, 24, 36, 56, 8, colorPrimary);
            gfx.drawRoundRect(6, 26, 32, 52, 6, colorAccent);
            
            // Tube 2 (Mins): x=46 to x=82
            gfx.drawRoundRect(46, 24, 36, 56, 8, colorPrimary);
            gfx.drawRoundRect(48, 26, 32, 52, 6, colorAccent);

            // Tube 3 (Secs): x=88 to x=124
            gfx.drawRoundRect(88, 24, 36, 56, 8, colorPrimary);
            gfx.drawRoundRect(90, 26, 32, 52, 6, colorPrimary);

            // Cathode glowing filament coils at bottom of tubes
            gfx.drawFastHLine(8, 72, 28, colorPrimary);
            gfx.drawFastHLine(50, 72, 28, colorPrimary);
            gfx.drawFastHLine(92, 72, 28, colorAccent);

            // USSR Tech Telemetry Box
            gfx.drawRect(4, 86, 120, 68, colorAccent);
            gfx.setTextSize(1);
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(12, 92);
            gfx.print("IN-14 NIXIE CORE");
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(10, 108);
            gfx.printf("AMB TEMP : %.1f C", sensors.data.tempC);
            gfx.setCursor(10, 122);
            gfx.printf("OUT TEMP : %.1f C", w.tempC);
            gfx.setCursor(10, 136);
            gfx.printf("STATUS   : %-9s", w.condition.substring(0, 9).c_str());
        }

        // Centered Size 3 digits inside each 36px vacuum tube box!
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(5, 40);
        gfx.printf("%02d", h);
        
        gfx.setCursor(47, 40);
        gfx.printf("%02d", m);

        gfx.setTextColor(colorAccent, colorBg);
        gfx.setCursor(89, 40);
        gfx.printf("%02d", s);
    }

    // =========================================================
    // ⌚ NEW: Style 4: Iconic Casio F-91W Digital Chronograph
    // =========================================================
    void renderRetroFlip(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            // Draw flip cards
            gfx.fillRoundRect(10, 40, 48, 60, 4, 0x3186); // Dark grey
            gfx.fillRoundRect(70, 40, 48, 60, 4, 0x3186);
            // Center split line
            gfx.drawLine(8, 70, 120, 70, colorBg);
        }
        
        gfx.setTextColor(0xFFFF, 0x3186); // White text on dark grey
        gfx.setTextSize(4);
        gfx.setCursor(14, 55);
        gfx.printf("%02d", h);
        gfx.setCursor(74, 55);
        gfx.printf("%02d", m);
        
        // Seconds ticking below
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(1);
        gfx.setCursor(55, 110);
        gfx.printf(":%02d", s);
    }

    // =========================================================
    // 🛡️ NEW: Style 5: Casio G-Shock Databank Tactical HUD
    // =========================================================
    void renderBinaryMatrix(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, 0x0000); // Black background
            gfx.setTextColor(0x07E0); // Bright green
            gfx.setTextSize(1);
            gfx.setCursor(10, 20);
            gfx.print("H M S");
            gfx.drawLine(10, 30, 118, 30, 0x07E0);
        }
        
        uint16_t c_on = 0x07E0;
        uint16_t c_off = 0x0180; // Dark green
        
        int vals[3] = {h, m, s};
        int x_offsets[3] = {16, 56, 96};
        
        for (int c = 0; c < 3; c++) {
            for (int r = 0; r < 6; r++) {
                int bit = (vals[c] >> (5 - r)) & 1;
                uint16_t color = bit ? c_on : c_off;
                gfx.fillCircle(x_offsets[c], 45 + r * 15, 5, color);
            }
        }
    }

    // =========================================================
    // 🧮 NEW: Style 6: Casio CA-53W Calculator Databank Watch
    // =========================================================
    void renderCRTTerminal(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, 0x0000);
            for (int i=16; i<160; i+=4) {
                gfx.drawFastHLine(0, i, 128, 0x0180); // scanlines
            }
        }
        gfx.setTextColor(0x07E0, 0x0000);
        gfx.setTextSize(1);
        gfx.setCursor(10, 40);
        gfx.print("user@desky:~$ time");
        
        gfx.setTextSize(2);
        gfx.setCursor(10, 60);
        gfx.printf("%02d:%02d:%02d", h, m, s);
        
        gfx.setTextSize(1);
        gfx.setCursor(10, 90);
        if (s % 2 == 0) gfx.print("_"); else gfx.print(" ");
    }

    // =========================================================
    // 🗺️ NEW: Style 7: Casio Royale (AE-1200 World Time Radar)
    // =========================================================
    void renderOrbitalRings(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        int cx = 64, cy = 88;
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            gfx.drawCircle(cx, cy, 40, 0x2104);
            gfx.drawCircle(cx, cy, 30, 0x2104);
            gfx.drawCircle(cx, cy, 20, 0x2104);
        }
        
        // Simple orbital pointers
        // Erase old
        if (lastS >= 0) {
            float os = lastS * M_PI / 30.0 - M_PI / 2.0;
            gfx.fillCircle(cx + cos(os)*40, cy + sin(os)*40, 3, 0x2104);
            float om = lastM * M_PI / 30.0 - M_PI / 2.0;
            gfx.fillCircle(cx + cos(om)*30, cy + sin(om)*30, 4, 0x2104);
            float oh = (lastH%12) * M_PI / 6.0 - M_PI / 2.0;
            gfx.fillCircle(cx + cos(oh)*20, cy + sin(oh)*20, 5, 0x2104);
        }
        
        float ah = (h%12) * M_PI / 6.0 - M_PI / 2.0;
        gfx.fillCircle(cx + cos(ah)*20, cy + sin(ah)*20, 5, colorAccent);
        
        float am = m * M_PI / 30.0 - M_PI / 2.0;
        gfx.fillCircle(cx + cos(am)*30, cy + sin(am)*30, 4, colorPrimary);
        
        float a_s = s * M_PI / 30.0 - M_PI / 2.0;
        gfx.fillCircle(cx + cos(a_s)*40, cy + sin(a_s)*40, 3, colorText);
        
        gfx.setTextSize(1);
        gfx.setTextColor(colorText, colorBg);
        gfx.setCursor(cx-18, cy-4);
        gfx.printf("%02d:%02d", h, m);
    }

    // =========================================================
    // ✈️ NEW: Style 8: Aviation Altimeter
    // =========================================================
    void renderAviationAltimeter(Adafruit_GFX& gfx, int h, int m, int s,
                          const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                          uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        int cx = 64, cy = 88;
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, 0x0000); // Pitch black
            
            // Outer casing
            gfx.fillCircle(cx, cy, 60, 0x18E3); // Dark grey
            gfx.fillCircle(cx, cy, 56, 0x0000); // Black dial
            
            // Altimeter ticks
            for (int i = 0; i < 60; i++) {
                float a = i * (2.0f * M_PI / 60.0f) - M_PI / 2.0f;
                int len = (i % 5 == 0) ? 8 : 4;
                int tx1 = cx + (int)(cos(a) * (54 - len));
                int ty1 = cy + (int)(sin(a) * (54 - len));
                int tx2 = cx + (int)(cos(a) * 54);
                int ty2 = cy + (int)(sin(a) * 54);
                uint16_t c = (i % 5 == 0) ? 0xFFFF : 0x7BEF;
                gfx.drawLine(tx1, ty1, tx2, ty2, c);
            }
            
            gfx.setTextColor(0xFFFF);
            gfx.setTextSize(1);
            gfx.setCursor(40, 50);
            gfx.print("ALTITUDE");
        }
        
        // Redraw inner dial to clear old hands
        gfx.fillCircle(cx, cy, 45, 0x0000);
        
        // Redraw inner texts that got cleared
        gfx.setTextColor(0xFFFF);
        gfx.setTextSize(1);
        gfx.setCursor(40, 50);
        gfx.print("ALTITUDE");

        gfx.fillRect(cx - 15, cy + 15, 30, 12, 0x18E3);
        gfx.setCursor(cx - 12, cy + 17);
        gfx.printf("%04d", (int)sensors.data.pressureHpa);
        
        // Hour hand
        float hAngle = (h % 12 + m / 60.0f) * (2.0f * M_PI / 12.0f) - M_PI / 2.0f;
        gfx.drawLine(cx, cy, cx + (int)(cos(hAngle) * 25), cy + (int)(sin(hAngle) * 25), 0xFFFF);
        gfx.drawLine(cx+1, cy, cx+1 + (int)(cos(hAngle) * 25), cy + (int)(sin(hAngle) * 25), 0xFFFF);
        
        // Minute hand
        float mAngle = (m + s / 60.0f) * (2.0f * M_PI / 60.0f) - M_PI / 2.0f;
        gfx.drawLine(cx, cy, cx + (int)(cos(mAngle) * 38), cy + (int)(sin(mAngle) * 38), 0xFFFF);
        
        // Second hand
        float sAngle = s * (2.0f * M_PI / 60.0f) - M_PI / 2.0f;
        gfx.drawLine(cx, cy, cx + (int)(cos(sAngle) * 42), cy + (int)(sin(sAngle) * 42), 0xF800); // Red
        gfx.fillCircle(cx, cy, 3, 0xFFFF);
    }

    // =========================================================
    // 📰 NEW: Style 9: Minimalist E-Ink (High Contrast)
    // =========================================================
    void renderMinimalistEink(Adafruit_GFX& gfx, int h, int m, int s,
                         const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                         uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        uint16_t bg = 0xFFFF; // White
        uint16_t fg = 0x0000; // Black
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, bg);
            
            // Top dividing line
            gfx.drawLine(10, 30, 118, 30, fg);
            gfx.setTextColor(fg, bg);
            gfx.setTextSize(1);
            gfx.setCursor(10, 20);
            gfx.print("DESKY");
            
            // Bottom dividing line
            gfx.drawLine(10, 130, 118, 130, fg);
        }
        
        gfx.setTextColor(fg, bg);
        
        // Pomodoro status
        gfx.setCursor(10, 136);
        gfx.fillRect(10, 136, 108, 10, bg); // Clear old text
        if ((pomo.state != POMO_IDLE)) {
            if ((pomo.state == POMO_WORK)) gfx.printf("FOCUS: %02d:%02d", pomo.remainingSec / 60, pomo.remainingSec % 60);
            else gfx.printf("BREAK: %02d:%02d", pomo.remainingSec / 60, pomo.remainingSec % 60);
        } else {
            gfx.print("IDLE");
        }
        
        // Time
        gfx.setTextSize(3);
        gfx.setCursor(10, 50);
        gfx.printf("%02d:%02d", h, m);
        
        gfx.setTextSize(2);
        gfx.setCursor(100, 56);
        gfx.fillRect(100, 56, 24, 16, bg);
        gfx.printf("%02d", s);
        
        // Weather
        gfx.setTextSize(1);
        gfx.setCursor(10, 100);
        gfx.fillRect(10, 100, 100, 20, bg);
        gfx.printf("OUT: %.1fC", w.tempC);
        gfx.setCursor(10, 112);
        gfx.printf("IN: %.1fC", sensors.data.tempC);
    }

    // =========================================================
    // 🪙 Style 10: Casio A168 Vintage ElectroLuminescence (Gold/Silver Edition)
    // =========================================================
    void renderSteampunkGauge(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        int cx = 64, cy = 88;
        uint16_t copper = 0xFD20; // Orange/Brownish
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, 0x18C3); // Dark slate
            gfx.fillCircle(cx, cy, 45, 0x0000);
            gfx.drawCircle(cx, cy, 45, copper);
            gfx.drawCircle(cx, cy, 44, copper);
            for(int i=0; i<360; i+=15) {
                float rad = i * M_PI / 180.0;
                gfx.drawLine(cx + cos(rad)*40, cy + sin(rad)*40, cx + cos(rad)*44, cy + sin(rad)*44, copper);
            }
        }
        
        // Erase
        if (lastS >= 0) {
            float os = lastS * M_PI / 30.0 - M_PI / 2.0;
            gfx.drawLine(cx, cy, cx + cos(os)*38, cy + sin(os)*38, 0x0000);
            float om = lastM * M_PI / 30.0 - M_PI / 2.0;
            gfx.drawLine(cx, cy, cx + cos(om)*30, cy + sin(om)*30, 0x0000);
            float oh = (lastH%12) * M_PI / 6.0 - M_PI / 2.0;
            gfx.drawLine(cx, cy, cx + cos(oh)*20, cy + sin(oh)*20, 0x0000);
        }
        
        float ah = (h%12) * M_PI / 6.0 - M_PI / 2.0;
        gfx.drawLine(cx, cy, cx + cos(ah)*20, cy + sin(ah)*20, 0xFFFF);
        float am = m * M_PI / 30.0 - M_PI / 2.0;
        gfx.drawLine(cx, cy, cx + cos(am)*30, cy + sin(am)*30, 0xCE79);
        float a_s = s * M_PI / 30.0 - M_PI / 2.0;
        gfx.drawLine(cx, cy, cx + cos(a_s)*38, cy + sin(a_s)*38, copper);
        gfx.fillCircle(cx, cy, 4, copper);
    }

    // =========================================================
    // 📞 Style 11: Casio DB-360 Databank Telememo 30
    // =========================================================
    void renderSegmentLED(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, 0x0000);
        }
        
        gfx.setTextColor(0xF800, 0x0000); // Pure Red
        gfx.setTextSize(4);
        gfx.setCursor(8, 50);
        gfx.printf("%02d", h);
        
        if (s % 2 == 0) gfx.print(":"); else gfx.print(" ");
        gfx.setCursor(68, 50);
        gfx.printf("%02d", m);
        
        gfx.setTextSize(2);
        gfx.setCursor(55, 100);
        gfx.printf("%02d", s);
    }

    // =========================================================
    // 🌃 Style 12: Cyberpunk 2077 Night City HUD Chronograph
    // =========================================================
    void renderCyberpunk2077(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, ST77XX_BLACK);

            // Cyberpunk Angular Border Accents
            gfx.drawFastHLine(0, 16, 128, ST77XX_YELLOW);
            gfx.drawFastHLine(0, 159, 128, ST77XX_YELLOW);

            gfx.drawTriangle(4, 20, 20, 20, 4, 36, ST77XX_YELLOW);
            gfx.drawTriangle(124, 20, 108, 20, 124, 36, ST77XX_CYAN);

            // Top Header
            gfx.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
            gfx.setTextSize(1);
            gfx.setCursor(24, 24);
            gfx.print("CYBERPUNK 2077");

            // Telemetry gauges
            gfx.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
            gfx.setCursor(10, 38);
            gfx.printf("CPU:240M HEAP:%dK", ESP.getFreeHeap() / 1024);

            gfx.drawRect(8, 48, 112, 6, ST77XX_YELLOW);
            int barW = (s * 110) / 59;
            gfx.fillRect(9, 49, barW, 4, ST77XX_CYAN);

            // Main Time Outer Box (Angular neon cyan)
            gfx.drawRoundRect(6, 60, 116, 62, 6, ST77XX_CYAN);
            gfx.drawRoundRect(7, 61, 114, 60, 5, ST77XX_YELLOW);

            gfx.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
            gfx.setCursor(12, 64);
            gfx.print("NIGHT CITY // CHRONO");

            // Footer
            gfx.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
            gfx.setCursor(10, 128);
            gfx.printf("TEMP: %.1fC | OUT: %.1fC", sensors.data.tempC, w.valid ? w.tempC : 0.0f);
            gfx.setCursor(10, 142);
            gfx.print("SYSTEM STATUS: ONLINE");
        }

        // Glitching Cyberpunk Digital Time
        gfx.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
        gfx.setTextSize(3);
        gfx.setCursor(12, 82);
        if (s % 2 == 0) {
            gfx.printf("%02d:%02d", h, m);
        } else {
            gfx.printf("%02d %02d", h, m);
        }

        gfx.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
        gfx.setTextSize(2);
        gfx.setCursor(94, 88);
        gfx.printf("%02d", s);
    }


    // =========================================================
    // ⏱️ Style 13: Classic Chronograph
    // =========================================================
    void renderChronograph(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        int cx = 120, cy = 120;
        int r = 110;
        uint16_t TFT_SILVER = 0xC618;
        uint16_t TFT_DARKGREY = 0x7BEF;
        uint16_t TFT_LIGHTGREY = 0xD69A;
        
        if (fullRedraw) {
            gfx.fillScreen(0x0000); // Black
            
            // Outer dial
            gfx.drawCircle(cx, cy, r, TFT_SILVER);
            gfx.drawCircle(cx, cy, r - 2, TFT_DARKGREY);

            // Tick marks
            for (int i = 0; i < 60; i++) {
                float angle = i * 6.0f * (M_PI / 180.0f);
                int len = (i % 5 == 0) ? 10 : 4;
                uint16_t color = (i % 5 == 0) ? 0xFFFF : TFT_LIGHTGREY;
                gfx.drawLine(cx + cos(angle) * (r - len), cy + sin(angle) * (r - len),
                             cx + cos(angle) * r, cy + sin(angle) * r, color);
            }

            // Sub-dials
            int sdR = 20;
            // Sub-dial 1 (Left - Temp)
            gfx.drawCircle(cx - 40, cy, sdR, TFT_DARKGREY);
            gfx.setTextColor(TFT_LIGHTGREY);
            gfx.setTextSize(1);
            gfx.setCursor(cx - 52, cy + sdR + 2); gfx.print("TEMP");

            // Sub-dial 2 (Right - Humidity)
            gfx.drawCircle(cx + 40, cy, sdR, TFT_DARKGREY);
            gfx.setCursor(cx + 32, cy + sdR + 2); gfx.print("HUM");
        }
        
        int sdR = 20;
        // Erase old hands
        if (lastS >= 0) {
            // Main second hand
            float osAngle = lastS * 6.0f * (M_PI / 180.0f) - M_PI / 2.0f;
            gfx.drawLine(cx - cos(osAngle) * 15, cy - sin(osAngle) * 15, cx + cos(osAngle) * 90, cy + sin(osAngle) * 90, 0x0000);
            
            // Minute hand
            float omAngle = (lastM + lastS / 60.0f) * 6.0f * (M_PI / 180.0f) - M_PI / 2.0f;
            gfx.drawLine(cx, cy, cx + cos(omAngle) * 80, cy + sin(omAngle) * 80, 0x0000);
            gfx.drawLine(cx + 1, cy, cx + 1 + cos(omAngle) * 80, cy + sin(omAngle) * 80, 0x0000);
            
            // Hour hand
            float ohAngle = (lastH % 12 + lastM / 60.0f) * 30.0f * (M_PI / 180.0f) - M_PI / 2.0f;
            gfx.drawLine(cx, cy, cx + cos(ohAngle) * 50, cy + sin(ohAngle) * 50, 0x0000);
            gfx.drawLine(cx + 1, cy, cx + 1 + cos(ohAngle) * 50, cy + sin(ohAngle) * 50, 0x0000);
            
            // Temp hand (static mostly, but we redraw it anyway)
            float otempAngle = map(sensors.data.tempC, 0, 50, 0, 360) * (M_PI / 180.0f) - M_PI / 2.0f;
            gfx.drawLine(cx - 40, cy, cx - 40 + cos(otempAngle) * (sdR - 4), cy + sin(otempAngle) * (sdR - 4), 0x0000);
            
            // Hum hand
            float ohumAngle = map(sensors.data.humidity, 0, 100, 0, 360) * (M_PI / 180.0f) - M_PI / 2.0f;
            gfx.drawLine(cx + 40, cy, cx + 40 + cos(ohumAngle) * (sdR - 4), cy + sin(ohumAngle) * (sdR - 4), 0x0000);
        }

        // Draw new hands
        float tempAngle = map(sensors.data.tempC, 0, 50, 0, 360) * (M_PI / 180.0f) - M_PI / 2.0f;
        gfx.drawLine(cx - 40, cy, cx - 40 + cos(tempAngle) * (sdR - 4), cy + sin(tempAngle) * (sdR - 4), 0xF800); // Red
        
        float humAngle = map(sensors.data.humidity, 0, 100, 0, 360) * (M_PI / 180.0f) - M_PI / 2.0f;
        gfx.drawLine(cx + 40, cy, cx + 40 + cos(humAngle) * (sdR - 4), cy + sin(humAngle) * (sdR - 4), 0x07FF); // Cyan
        
        float hAngle = (h % 12 + m / 60.0f) * 30.0f * (M_PI / 180.0f) - M_PI / 2.0f;
        float mAngle = (m + s / 60.0f) * 6.0f * (M_PI / 180.0f) - M_PI / 2.0f;
        float sAngle = s * 6.0f * (M_PI / 180.0f) - M_PI / 2.0f;

        // Draw hour hand
        gfx.drawLine(cx, cy, cx + cos(hAngle) * 50, cy + sin(hAngle) * 50, 0xFFFF);
        gfx.drawLine(cx + 1, cy, cx + 1 + cos(hAngle) * 50, cy + sin(hAngle) * 50, 0xFFFF);
        
        // Draw minute hand
        gfx.drawLine(cx, cy, cx + cos(mAngle) * 80, cy + sin(mAngle) * 80, 0xFFFF);
        gfx.drawLine(cx + 1, cy, cx + 1 + cos(mAngle) * 80, cy + sin(mAngle) * 80, 0xFFFF);

        // Draw second hand
        gfx.drawLine(cx - cos(sAngle) * 15, cy - sin(sAngle) * 15, cx + cos(sAngle) * 90, cy + sin(sAngle) * 90, 0xF800);
        gfx.fillCircle(cx, cy, 4, 0xF800);
    }

    // =========================================================
    // 📐 Style 14: Bauhaus Minimal
    // =========================================================
    void renderBauhaus(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, const SensorManager& sensors, const PomodoroTimer& pomo,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        int cx = 120, cy = 120;
        int r = 110;
        
        if (fullRedraw) {
            gfx.fillScreen(0x18E3); // Very dark gray/blue
            // Just minimal markers for 12, 3, 6, 9
            gfx.fillRect(cx - 2, cy - r, 4, 15, 0xF800); // Red at 12
            gfx.fillRect(cx + r - 15, cy - 2, 15, 4, 0xFFFF); // White
            gfx.fillRect(cx - 2, cy + r - 15, 4, 15, 0xFFFF);
            gfx.fillRect(cx - r, cy - 2, 15, 4, 0xFFFF);
        }

        // Erase old
        if (lastS >= 0) {
            float ohAngle = (lastH % 12 + lastM / 60.0f) * 30.0f * (M_PI / 180.0f) - M_PI / 2.0f;
            float omAngle = (lastM + lastS / 60.0f) * 6.0f * (M_PI / 180.0f) - M_PI / 2.0f;
            float osAngle = lastS * 6.0f * (M_PI / 180.0f) - M_PI / 2.0f;
            
            float hx = cos(ohAngle), hy = sin(ohAngle);
            for(int w = -3; w <= 3; w++) {
                gfx.drawLine(cx + hy * w, cy - hx * w, cx + hx * 60 + hy * w, cy + hy * 60 - hx * w, 0x18E3);
            }
            float mx = cos(omAngle), my = sin(omAngle);
            for(int w = -1; w <= 1; w++) {
                gfx.drawLine(cx + my * w, cy - mx * w, cx + mx * 95 + my * w, cy + my * 95 - mx * w, 0x18E3);
            }
            gfx.drawLine(cx, cy, cx + cos(osAngle) * 105, cy + sin(osAngle) * 105, 0x18E3);
        }

        float hAngle = (h % 12 + m / 60.0f) * 30.0f * (M_PI / 180.0f) - M_PI / 2.0f;
        float mAngle = (m + s / 60.0f) * 6.0f * (M_PI / 180.0f) - M_PI / 2.0f;
        float sAngle = s * 6.0f * (M_PI / 180.0f) - M_PI / 2.0f;

        // Hour hand (thick, blocky)
        float hx = cos(hAngle), hy = sin(hAngle);
        for(int w = -3; w <= 3; w++) {
            gfx.drawLine(cx + hy * w, cy - hx * w, cx + hx * 60 + hy * w, cy + hy * 60 - hx * w, 0xFFFF);
        }

        // Minute hand (thinner, longer)
        float mx = cos(mAngle), my = sin(mAngle);
        for(int w = -1; w <= 1; w++) {
            gfx.drawLine(cx + my * w, cy - mx * w, cx + mx * 95 + my * w, cy + my * 95 - mx * w, 0xFFFF);
        }

        // Second hand (red, very thin)
        gfx.drawLine(cx, cy, cx + cos(sAngle) * 105, cy + sin(sAngle) * 105, 0xF800);
        
        // Center cap
        gfx.fillCircle(cx, cy, 6, 0xFFFF);
        gfx.fillCircle(cx, cy, 2, 0x0000);
    }
};

#endif // WATCHFACE_ENGINE_H
