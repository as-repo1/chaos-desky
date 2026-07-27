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
    WATCHFACE_CASIO_GSHOCK = 5,
    WATCHFACE_CASIO_CALCULATOR = 6,
    WATCHFACE_CASIO_ROYALE = 7,
    WATCHFACE_SEIKO_DIVER = 8,
    WATCHFACE_PULSAR_LED = 9
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
            case WATCHFACE_CASIO_CALCULATOR:
                renderCasioCalculator(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CASIO_ROYALE:
                renderCasioRoyale(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_SEIKO_DIVER:
                renderSeikoDiver(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_PULSAR_LED:
                renderPulsarLED(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
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

    // --- Style 2: Modern Minimalist Digital (Fixed UI & Zero Overlap) ---
    void renderModernDigital(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, float tempC,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            
            // Clean top title badge
            gfx.fillRoundRect(6, 20, 116, 18, 4, colorPrimary);
            gfx.setTextColor(colorBg, colorPrimary);
            gfx.setTextSize(1);
            gfx.setCursor(14, 25);
            gfx.print("MODERN DIGITAL HUD");

            // Main digital time container frame
            gfx.drawRoundRect(4, 44, 120, 48, 6, colorAccent);
            gfx.drawRoundRect(5, 45, 118, 46, 5, colorPrimary);
            
            // Environmental Telemetry Box below
            gfx.drawRoundRect(4, 98, 120, 56, 6, colorPrimary);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(12, 106);
            gfx.printf("CITY : %-10s", w.cityName.substring(0, 10).c_str());
            gfx.setCursor(12, 120);
            gfx.printf("TEMP : %.1fC / %.1fC", tempC, w.tempC);
            gfx.setCursor(12, 134);
            gfx.printf("WIND : %.1f m/s (%s)", w.windSpeedMs, w.condition.substring(0, 4).c_str());
        }

        // Clean time rendering with exact zero-overlap margins
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(10, 56);
        gfx.printf("%02d:%02d", h, m);

        // Separated sleek second pillbox
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(2);
        gfx.setCursor(98, 62);
        gfx.printf("%02d", s);
    }

    // --- Style 3: Retro Neon Nixie Tube (Fixed Geometry & Glass Glow) ---
    void renderNeonNixie(Adafruit_GFX& gfx, int h, int m, int s,
                         const OutdoorWeatherData& w, float tempC,
                         uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            
            // 3 Staggered Nixie Glass Vacuum Tubes (Each 40px wide, zero clash)
            // Tube 1 (Hours): x=2 to x=42
            gfx.drawRoundRect(2, 24, 40, 56, 8, colorPrimary);
            gfx.drawRoundRect(4, 26, 36, 52, 6, colorAccent);
            
            // Tube 2 (Mins): x=44 to x=84
            gfx.drawRoundRect(44, 24, 40, 56, 8, colorPrimary);
            gfx.drawRoundRect(46, 26, 36, 52, 6, colorAccent);

            // Tube 3 (Secs): x=86 to x=126
            gfx.drawRoundRect(86, 24, 40, 56, 8, colorPrimary);
            gfx.drawRoundRect(88, 26, 36, 52, 6, colorPrimary);

            // Cathode glowing filament coils at bottom of tubes
            gfx.drawFastHLine(8, 72, 28, colorPrimary);
            gfx.drawFastHLine(50, 72, 28, colorPrimary);
            gfx.drawFastHLine(92, 72, 28, colorAccent);

            // USSR Tech Telemetry Box
            gfx.drawRect(6, 88, 116, 64, colorAccent);
            gfx.setTextSize(1);
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(14, 94);
            gfx.print("[- IN-14 NIXIE CORE -]");
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(14, 110);
            gfx.printf("AMB TEMP : %.1f C", tempC);
            gfx.setCursor(14, 124);
            gfx.printf("OUT TEMP : %.1f C", w.tempC);
            gfx.setCursor(14, 138);
            gfx.printf("STATUS   : %-10s", w.condition.c_str());
        }

        // Perfectly centered Size 3 digits inside each 40px vacuum tube box!
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(6, 40);
        gfx.printf("%02d", h);
        
        gfx.setCursor(48, 40);
        gfx.printf("%02d", m);

        gfx.setTextColor(colorAccent, colorBg);
        gfx.setCursor(90, 40);
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

    // =========================================================
    // 🧮 NEW: Style 6: Casio CA-53W Calculator Databank Watch
    // =========================================================
    void renderCasioCalculator(Adafruit_GFX& gfx, int h, int m, int s,
                               const OutdoorWeatherData& w, float tempC,
                               uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);

            // Outer rectangular Calculator case
            gfx.drawRoundRect(4, 18, 120, 140, 4, colorPrimary);
            
            // Recessed LCD Top Screen
            gfx.drawRect(8, 22, 112, 42, colorAccent);
            gfx.drawRect(9, 23, 110, 40, colorPrimary);

            // LCD indicators
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setTextSize(1);
            gfx.setCursor(14, 26);
            gfx.print("ALM SIG DATABANK");

            // Brand Divider text
            gfx.setTextColor(colorPrimary, colorBg);
            gfx.setCursor(16, 68);
            gfx.print("CASIO  WATER WR RESIST");

            // Miniature 4x4 Calculator Keypad Graphics!
            int startY = 80;
            const char* keys[4][4] = {
                {"7", "8", "9", "/"},
                {"4", "5", "6", "*"},
                {"1", "2", "3", "-"},
                {"0", ".", "=", "+"}
            };
            
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    int bx = 10 + c * 27;
                    int by = startY + r * 18;
                    gfx.drawRoundRect(bx, by, 25, 15, 2, colorPrimary);
                    // Color code math operators differently!
                    if (c == 3 || (r == 3 && c == 2)) {
                        gfx.setTextColor(colorAccent, colorBg);
                    } else {
                        gfx.setTextColor(colorText, colorBg);
                    }
                    gfx.setCursor(bx + 9, by + 4);
                    gfx.print(keys[r][c]);
                }
            }
        }

        // Overdraw time inside top LCD window
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(14, 37);
        gfx.printf("%02d:%02d", h, m);

        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(2);
        gfx.setCursor(94, 43);
        gfx.printf("%02d", s);
    }

    // =========================================================
    // 🗺️ NEW: Style 7: Casio Royale (AE-1200 World Time Radar)
    // =========================================================
    void renderCasioRoyale(Adafruit_GFX& gfx, int h, int m, int s,
                           const OutdoorWeatherData& w, float tempC,
                           uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);

            // Outer Casio Royale Chassis
            gfx.drawRoundRect(4, 18, 120, 140, 6, colorPrimary);
            
            // Top Left: World Time Radar Round Globe Window
            gfx.drawCircle(32, 46, 22, colorPrimary);
            gfx.drawCircle(32, 46, 21, colorAccent);
            gfx.drawFastHLine(10, 46, 44, colorPrimary);
            gfx.drawFastVLine(32, 24, 44, colorPrimary);

            // Top Right: Status indicators and World Time Zone Box
            gfx.drawRect(58, 24, 60, 44, colorPrimary);
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setTextSize(1);
            gfx.setCursor(62, 28);
            gfx.print("WORLD TIME");
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(62, 40);
            gfx.print("LON NYC TYO");
            gfx.setCursor(62, 52);
            gfx.printf("TMP:%.0fC", tempC);

            // Bottom Panoramic Main Display Frame
            gfx.drawRoundRect(8, 74, 112, 58, 4, colorAccent);
            gfx.drawRoundRect(9, 75, 110, 56, 3, colorPrimary);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(14, 80);
            gfx.print("5 ALARMS   10 YEAR BAT");
            
            // Footer branding
            gfx.setTextColor(colorPrimary, colorBg);
            gfx.setCursor(20, 142);
            gfx.print("ILLUMINATOR - WR100M");
        }

        // Radar scan pulse (dynamic radar beam in the circle!)
        float angleR = s * (2.0f * M_PI / 15.0f);
        int rx = 32 + (int)(cos(angleR) * 16);
        int ry = 46 + (int)(sin(angleR) * 16);
        gfx.fillCircle(32, 46, 3, colorAccent);
        gfx.fillCircle(rx, ry, 2, colorText);

        // Big Main LCD Time
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(14, 96);
        if (s % 2 == 0) {
            gfx.printf("%02d:%02d", h, m);
        } else {
            gfx.printf("%02d %02d", h, m);
        }

        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(2);
        gfx.setCursor(94, 102);
        gfx.printf("%02d", s);
    }

    // =========================================================
    // 🌊 NEW: Style 8: Seiko Diver (Luminescent Prospex Marine)
    // =========================================================
    void renderSeikoDiver(Adafruit_GFX& gfx, int h, int m, int s,
                          const OutdoorWeatherData& w, float tempC,
                          uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            
            // Dive rotating Bezel frame
            gfx.drawCircle(64, 76, 54, colorPrimary);
            gfx.drawCircle(64, 76, 52, colorAccent);
            
            // Bezel ticks (0, 15, 30, 45 marine markings)
            for (int i = 0; i < 12; i++) {
                float a = i * (2.0f * M_PI / 12.0f);
                int tx1 = 64 + (int)(cos(a) * 46);
                int ty1 = 76 + (int)(sin(a) * 46);
                int tx2 = 64 + (int)(cos(a) * 50);
                int ty2 = 76 + (int)(sin(a) * 50);
                gfx.drawLine(tx1, ty1, tx2, ty2, colorAccent);
            }
            // Triangle marking at 12 o'clock (top)
            gfx.fillTriangle(64, 24, 58, 32, 70, 32, colorPrimary);

            // Seiko Marine badge
            gfx.setTextColor(colorPrimary, colorBg);
            gfx.setTextSize(1);
            gfx.setCursor(48, 38);
            gfx.print("SEIKO");
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(24, 110);
            gfx.print("DIVER'S 200m AUTOMATIC");
            
            // Telemetry sub-box
            gfx.drawRoundRect(14, 134, 100, 22, 4, colorPrimary);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(22, 141);
            gfx.printf("TEMP: %.1fC DEPTH: 0m", tempC);
        }

        // Bold Luminescent Digital Chronometer Display in center
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(18, 66);
        gfx.printf("%02d:%02d", h, m);

        // Seconds indicator on right
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(2);
        gfx.setCursor(98, 72);
        gfx.printf("%02d", s);
    }

    // =========================================================
    // 🔴 NEW: Style 9: 1972 Hamilton Pulsar (Vintage Ruby LED)
    // =========================================================
    void renderPulsarLED(Adafruit_GFX& gfx, int h, int m, int s,
                         const OutdoorWeatherData& w, float tempC,
                         uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        
        if (fullRedraw) {
            // Fill with deep contrast background
            gfx.fillRect(0, 16, 128, 144, colorBg);

            // Cushion curved Gold/Stainless steel Pulsar case frame
            gfx.drawRoundRect(6, 24, 116, 124, 24, colorPrimary);
            gfx.drawRoundRect(8, 26, 112, 120, 22, colorPrimary);
            
            // Deep red tinted glass filter viewing screen
            gfx.drawRoundRect(16, 52, 96, 56, 10, colorAccent);
            gfx.drawRoundRect(17, 53, 94, 54, 9, colorAccent);

            // Vintage branding above and below glass window
            gfx.setTextColor(colorText, colorBg);
            gfx.setTextSize(1);
            gfx.setCursor(44, 36);
            gfx.print("PULSAR");
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(26, 116);
            gfx.print("TIME COMPUTER");
            
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(28, 130);
            gfx.printf("SOLID STATE %.0fC", tempC);
        }

        // Ruby Red glowing LED digital characters centered inside tinted glass!
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(22, 68);
        if (s % 2 == 0) {
            gfx.printf("%02d:%02d", h, m);
        } else {
            gfx.printf("%02d %02d", h, m);
        }

        gfx.setTextSize(2);
        gfx.setCursor(94, 74);
        gfx.printf("%02d", s);
    }
};

#endif // WATCHFACE_ENGINE_H
