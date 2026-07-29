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
    WATCHFACE_PULSAR_LED = 9,
    WATCHFACE_CASIO_A168 = 10,
    WATCHFACE_CASIO_DATABANK = 11,
    WATCHFACE_CYBERPUNK_2077 = 12
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
            case WATCHFACE_CASIO_A168:
                renderCasioA168(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CASIO_DATABANK:
                renderCasioDataBank(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CYBERPUNK_2077:
                renderCyberpunk2077(gfx, hours, mins, secs, weather, tempC, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
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
            gfx.printf("TEMP : %.1fC / %.1fC", tempC, w.tempC);
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
                         const OutdoorWeatherData& w, float tempC,
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
            gfx.printf("AMB TEMP : %.1f C", tempC);
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
            gfx.drawRect(8, 22, 112, 44, colorAccent);
            gfx.drawRect(9, 23, 110, 42, colorPrimary);

            // LCD indicators
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setTextSize(1);
            gfx.setCursor(10, 24);
            gfx.print("ALM SIG  DATABANK");

            // Brand Divider text
            gfx.setTextColor(colorPrimary, colorBg);
            gfx.setCursor(10, 68);
            gfx.print("CASIO  WR RESIST");

            // Miniature 4x4 Calculator Keypad Graphics!
            int startY = 78;
            const char* keys[4][4] = {
                {"7", "8", "9", "/"},
                {"4", "5", "6", "*"},
                {"1", "2", "3", "-"},
                {"0", ".", "=", "+"}
            };
            
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    int bx = 8 + c * 28;
                    int by = startY + r * 18;
                    gfx.drawRoundRect(bx, by, 26, 16, 2, colorPrimary);
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
        gfx.setCursor(10, 36);
        gfx.printf("%02d:%02d", h, m);

        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(2);
        gfx.setCursor(100, 42);
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
            gfx.drawCircle(28, 44, 18, colorPrimary);
            gfx.drawCircle(28, 44, 17, colorAccent);
            gfx.drawFastHLine(10, 44, 36, colorPrimary);
            gfx.drawFastVLine(28, 26, 36, colorPrimary);

            // Top Right: Status indicators and World Time Zone Box
            gfx.drawRect(52, 24, 68, 42, colorPrimary);
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setTextSize(1);
            gfx.setCursor(56, 28);
            gfx.print("WORLD TIME");
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(56, 38);
            gfx.print("NYC LON TYO");
            gfx.setCursor(56, 48);
            gfx.printf("TMP:%.0fC", tempC);

            // Bottom Panoramic Main Display Frame
            gfx.drawRoundRect(4, 70, 120, 64, 4, colorAccent);
            gfx.drawRoundRect(5, 71, 118, 62, 3, colorPrimary);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(10, 74);
            gfx.print("5 ALARMS  10Y BAT");
            
            // Footer branding
            gfx.setTextColor(colorPrimary, colorBg);
            gfx.setCursor(8, 142);
            gfx.print("ILLUMINATOR - WR");
        }

        // Radar scan pulse (dynamic radar beam in the circle!)
        float angleR = s * (2.0f * M_PI / 15.0f);
        int rx = 28 + (int)(cos(angleR) * 13);
        int ry = 44 + (int)(sin(angleR) * 13);
        gfx.fillCircle(28, 44, 2, colorAccent);
        gfx.fillCircle(rx, ry, 2, colorText);

        // Big Main LCD Time
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(10, 88);
        if (s % 2 == 0) {
            gfx.printf("%02d:%02d", h, m);
        } else {
            gfx.printf("%02d %02d", h, m);
        }

        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(2);
        gfx.setCursor(100, 94);
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
            gfx.drawCircle(64, 70, 44, colorPrimary);
            gfx.drawCircle(64, 70, 42, colorAccent);
            
            // Bezel ticks (0, 15, 30, 45 marine markings)
            for (int i = 0; i < 12; i++) {
                float a = i * (2.0f * M_PI / 12.0f);
                int tx1 = 64 + (int)(cos(a) * 36);
                int ty1 = 70 + (int)(sin(a) * 36);
                int tx2 = 64 + (int)(cos(a) * 40);
                int ty2 = 70 + (int)(sin(a) * 40);
                gfx.drawLine(tx1, ty1, tx2, ty2, colorAccent);
            }
            // Triangle marking at 12 o'clock (top)
            gfx.fillTriangle(64, 26, 58, 34, 70, 34, colorPrimary);

            // Seiko Marine badge
            gfx.setTextColor(colorPrimary, colorBg);
            gfx.setTextSize(1);
            gfx.setCursor(49, 38);
            gfx.print("SEIKO");
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(22, 94);
            gfx.print("DIVER'S 200m");
            
            // Telemetry sub-box
            gfx.drawRoundRect(6, 130, 116, 22, 4, colorPrimary);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(12, 137);
            gfx.printf("TEMP:%.1fC DEPTH:0m", tempC);
        }

        // Bold Luminescent Digital Chronometer Display in center
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(10, 58);
        gfx.printf("%02d:%02d", h, m);

        // Seconds indicator on right
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(2);
        gfx.setCursor(100, 64);
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

            // Cushion curved Gold/Stainless steel Pulsar case frame (filled for metal look)
            gfx.fillRoundRect(4, 20, 120, 132, 28, colorPrimary);
            gfx.drawRoundRect(4, 20, 120, 132, 28, colorAccent);
            gfx.drawRoundRect(6, 22, 116, 128, 26, colorText);
            
            // Side Command Button (Signature of Pulsar)
            gfx.fillRoundRect(122, 70, 6, 24, 3, colorAccent);

            // Deep red tinted glass filter viewing screen
            gfx.fillRoundRect(14, 50, 100, 52, 12, colorBg);
            gfx.drawRoundRect(14, 50, 100, 52, 12, colorAccent);
            gfx.drawRoundRect(15, 51, 98, 50, 11, colorAccent);

            // Vintage branding above and below glass window
            gfx.setTextColor(colorBg, colorPrimary);
            gfx.setTextSize(2);
            gfx.setCursor(30, 32);
            gfx.print("Pulsar");
            
            gfx.setTextColor(colorAccent, colorPrimary);
            gfx.setTextSize(1);
            gfx.setCursor(26, 110);
            gfx.print("TIME COMPUTER");
            
            gfx.setTextColor(colorText, colorPrimary);
            gfx.setCursor(22, 130);
            gfx.printf("SOLID STATE %.0fC", tempC);
        }

        // Ruby Red glowing LED digital characters centered inside tinted glass!
        // We force a deep ruby red for authenticity, or fallback to accent
        uint16_t ledColor = ((255 & 0xF8) << 8) | ((20 & 0xFC) << 3) | (40 >> 3);

        gfx.setTextColor(ledColor, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(18, 66);
        if (s % 2 == 0) {
            gfx.printf("%02d:%02d", h, m);
        } else {
            gfx.printf("%02d %02d", h, m);
        }

        gfx.setTextSize(2);
        gfx.setCursor(104, 74); // Adjusted for spacing
        gfx.printf("%02d", s);
    }

    // =========================================================
    // 🪙 Style 10: Casio A168 Vintage ElectroLuminescence (Gold/Silver Edition)
    // =========================================================
    void renderCasioA168(Adafruit_GFX& gfx, int h, int m, int s,
                         const OutdoorWeatherData& w, float tempC,
                         uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);

            // Double Metallic outer bezel frame
            gfx.drawRoundRect(4, 18, 120, 140, 8, colorPrimary);
            gfx.drawRoundRect(5, 19, 118, 138, 7, colorAccent);

            // Iconic ElectroLuminescence Header Banner
            gfx.fillRect(8, 24, 112, 16, colorPrimary);
            gfx.setTextColor(colorBg, colorPrimary);
            gfx.setTextSize(1);
            gfx.setCursor(12, 28);
            gfx.print("CASIO");
            gfx.setCursor(84, 28);
            gfx.print("A168W");

            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(10, 43);
            gfx.print("ELECTRO LUMINESCENCE");

            // Recessed Teal Cyan EL Backlit LCD Panel Frame
            gfx.drawRect(10, 56, 108, 64, colorAccent);
            gfx.drawRect(11, 57, 106, 62, colorPrimary);

            // LCD indicators line
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(14, 62);
            gfx.print("SU MO TU WE TH FR SA");

            // Footer markings
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(16, 124);
            gfx.print("WATER  WR  RESIST");
            gfx.drawFastHLine(14, 134, 100, colorPrimary);

            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(18, 142);
            gfx.printf("AMB TEMP: %.1fC", tempC);
        }

        // Full Cyan EL Glow box for time
        gfx.fillRect(14, 74, 100, 42, 0x03E0); // Deep Cyan EL LCD Panel
        gfx.setTextColor(ST77XX_BLACK, 0x03E0);
        gfx.setTextSize(3);
        gfx.setCursor(18, 84);
        if (s % 2 == 0) {
            gfx.printf("%02d:%02d", h, m);
        } else {
            gfx.printf("%02d %02d", h, m);
        }

        gfx.setTextSize(2);
        gfx.setCursor(92, 90);
        gfx.printf("%02d", s);
    }

    // =========================================================
    // 📞 Style 11: Casio DB-360 Databank Telememo 30
    // =========================================================
    void renderCasioDataBank(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, float tempC,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);

            // Databank rectangular chassis
            gfx.drawRoundRect(4, 18, 120, 140, 5, colorPrimary);

            // Header Banner
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setTextSize(1);
            gfx.setCursor(10, 24);
            gfx.print("CASIO  TELEMEMO 30");

            // Dot Matrix Sub-Window Frame
            gfx.drawRect(8, 36, 112, 42, colorPrimary);
            gfx.drawRect(9, 37, 110, 40, colorAccent);

            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(12, 42);
            gfx.print("NAME: CHAOS-DESKY");
            gfx.setCursor(12, 52);
            gfx.print("TEL : +888-ESP32");
            gfx.setCursor(12, 62);
            gfx.printf("MEM : [||||||..] %02d/30", (s % 30) + 1);

            // Lower Screen Frame
            gfx.drawRect(8, 82, 112, 56, colorAccent);
            gfx.setTextColor(colorPrimary, colorBg);
            gfx.setCursor(10, 86);
            gfx.print("AUTO SCHEDULE / ALM");

            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(10, 142);
            gfx.print("DATA BANK  WR 50M");
        }

        // Overdraw time inside bottom screen window
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(12, 102);
        gfx.printf("%02d:%02d", h, m);

        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(2);
        gfx.setCursor(96, 108);
        gfx.printf("%02d", s);
    }

    // =========================================================
    // 🌃 Style 12: Cyberpunk 2077 Night City HUD Chronograph
    // =========================================================
    void renderCyberpunk2077(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, float tempC,
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
            gfx.printf("TEMP: %.1fC | OUT: %.1fC", tempC, w.valid ? w.tempC : 0.0f);
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
};

#endif // WATCHFACE_ENGINE_H
