#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

struct Star3D {
    float x = 0;
    float y = 0;
    float z = 0;
};

struct Particle2D {
    int16_t x = 0;
    int16_t y = 0;
    int8_t speedY = 1;
};

// RGB565 Color Constants for Screensaver
#ifndef ST77XX_CYAN
#define ST77XX_CYAN    0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW  0xFFE0
#define ST77XX_GREEN   0x07E0
#define ST77XX_WHITE   0xFFFF
#endif

class ScreensaverEngine {
public:
    int animFrame = 0;
    int bounceX = 10;
    int bounceY = 30;
    int bounceDx = 2;
    int bounceDy = 2;

    Star3D stars[30];
    Particle2D hearts[4];

    int tftSaverMode = 0;   // 0: Cosmic Warp, 1: Matrix Rain, 2: Synthwave, 3: DVD Bounce
    int oledSaverMode = 0;  // 0: Cyber Cat, 1: Matrix Rain, 2: DVD Bounce, 3: 3D Tunnel

    // Matrix Rain Stream state
    int matrixY[10];
    int matrixSpeed[10];
    uint16_t dvdColor = ST77XX_CYAN;

    ScreensaverEngine() {
        for (int i = 0; i < 30; i++) {
            resetStar(i);
        }
        for (int i = 0; i < 4; i++) {
            hearts[i].x = 20 + (i * 25);
            hearts[i].y = 50 + (i * 12);
            hearts[i].speedY = 1 + (i % 2);
        }
        for (int i = 0; i < 10; i++) {
            matrixY[i] = random(-40, 0);
            matrixSpeed[i] = random(2, 5);
        }
    }

    void resetStar(int i) {
        stars[i].x = random(-60, 60);
        stars[i].y = random(-70, 70);
        stars[i].z = random(20, 100);
    }

    // Master render caller for TFT screensaver page
    void renderTftScreensaver(Adafruit_GFX& gfx, uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg) {
        // Auto-switch sub-mode every 300 frames (~15 seconds)
        if (animFrame % 300 == 0) {
            tftSaverMode = (tftSaverMode + 1) % 4;
        }

        switch (tftSaverMode) {
            case 0: renderTftCosmicWarp(gfx, colorPrimary, colorAccent, colorText, colorBg); break;
            case 1: renderTftMatrixRain(gfx, colorPrimary, colorBg); break;
            case 2: renderTftSynthwaveGrid(gfx, colorPrimary, colorAccent, colorText, colorBg); break;
            case 3: renderTftDvdLogo(gfx, colorBg); break;
            default: renderTftCosmicWarp(gfx, colorPrimary, colorAccent, colorText, colorBg); break;
        }
    }

    // Master render caller for OLED screensaver mode
    void renderOledScreensaver(Adafruit_SSD1306& oled) {
        if (animFrame % 300 == 0) {
            oledSaverMode = (oledSaverMode + 1) % 4;
        }

        switch (oledSaverMode) {
            case 0: renderOledCatMascot(oled); break;
            case 1: renderOledMatrixRain(oled); break;
            case 2: renderOledDvdBounce(oled); break;
            case 3: renderOled3DTunnel(oled); break;
            default: renderOledCatMascot(oled); break;
        }
    }

    // --- Render OLED Cute Cyber Cat Mascot (128x64) ---
    void renderOledCatMascot(Adafruit_SSD1306& oled) {
        oled.clearDisplay();

        // 1. Draw Floating Particles (Hearts/Stars)
        for (int i = 0; i < 4; i++) {
            hearts[i].y -= hearts[i].speedY;
            if (hearts[i].y < 4) {
                hearts[i].y = 58;
                hearts[i].x = random(10, 118);
            }
            int hx = hearts[i].x;
            int hy = hearts[i].y;
            oled.drawPixel(hx, hy, SSD1306_WHITE);
            oled.drawPixel(hx + 2, hy, SSD1306_WHITE);
            oled.drawPixel(hx + 1, hy + 1, SSD1306_WHITE);
        }

        // 2. Draw Cute Cat Head Silhouette
        int cx = 64;
        int cy = 34;

        // Ears
        oled.fillTriangle(cx - 18, cy - 10, cx - 28, cy - 24, cx - 10, cy - 16, SSD1306_WHITE);
        oled.fillTriangle(cx + 18, cy - 10, cx + 28, cy - 24, cx + 10, cy - 16, SSD1306_WHITE);

        // Head Round Rect
        oled.fillRoundRect(cx - 24, cy - 14, 48, 28, 8, SSD1306_WHITE);

        // Face Expressions (Frame switching)
        int subFrame = (animFrame / 4) % 3;
        
        if (subFrame == 0) { // Happy ^ _ ^
            oled.fillCircle(cx - 10, cy - 2, 4, SSD1306_BLACK);
            oled.fillCircle(cx + 10, cy - 2, 4, SSD1306_BLACK);
            oled.fillCircle(cx - 10, cy - 3, 2, SSD1306_WHITE);
            oled.fillCircle(cx + 10, cy - 3, 2, SSD1306_WHITE);
        } else if (subFrame == 1) { // Sleepy - _ -
            oled.drawFastHLine(cx - 14, cy - 2, 8, SSD1306_BLACK);
            oled.drawFastHLine(cx + 6, cy - 2, 8, SSD1306_BLACK);
        } else { // Cute Wink ^ _ ~
            oled.fillCircle(cx - 10, cy - 2, 4, SSD1306_BLACK);
            oled.fillCircle(cx - 10, cy - 3, 2, SSD1306_WHITE);
            oled.drawFastHLine(cx + 6, cy - 2, 8, SSD1306_BLACK);
        }

        // Cute Nose & Whiskers
        oled.fillTriangle(cx - 2, cy + 3, cx + 2, cy + 3, cx, cy + 5, SSD1306_BLACK);
        oled.drawFastHLine(cx - 20, cy + 2, 6, SSD1306_BLACK);
        oled.drawFastHLine(cx - 20, cy + 6, 6, SSD1306_BLACK);
        oled.drawFastHLine(cx + 14, cy + 2, 6, SSD1306_BLACK);
        oled.drawFastHLine(cx + 14, cy + 6, 6, SSD1306_BLACK);

        // Tail Wiggle
        int tailOffset = (animFrame % 2 == 0) ? -3 : 3;
        oled.drawLine(cx + 24, cy + 10, cx + 34 + tailOffset, cy + 18, SSD1306_WHITE);

        // Status Text
        oled.setCursor(18, 54);
        oled.print("~ MEOW DESKY ~");

        animFrame++;
    }

    // --- OLED Screensaver 2: Matrix Code Rain (128x64) ---
    void renderOledMatrixRain(Adafruit_SSD1306& oled) {
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setTextColor(SSD1306_WHITE);

        for (int i = 0; i < 10; i++) {
            int x = i * 13 + 2;
            matrixY[i] += matrixSpeed[i];
            if (matrixY[i] > 64) {
                matrixY[i] = random(-20, 0);
                matrixSpeed[i] = random(2, 5);
            }

            // Draw stream head
            oled.setCursor(x, matrixY[i]);
            char c = (char)random(33, 126);
            oled.print(c);

            // Trail line
            oled.drawFastVLine(x + 3, max(0, matrixY[i] - 16), 12, SSD1306_WHITE);
        }

        oled.drawFastHLine(0, 60, 128, SSD1306_WHITE);
        oled.setCursor(20, 52);
        oled.print("MATRIX CODE OLED");
        animFrame++;
    }

    // --- OLED Screensaver 3: DVD Bouncing Logo (128x64) ---
    void renderOledDvdBounce(Adafruit_SSD1306& oled) {
        oled.clearDisplay();

        bounceX += bounceDx;
        bounceY += bounceDy;

        if (bounceX <= 2 || bounceX >= 80) bounceDx = -bounceDx;
        if (bounceY <= 2 || bounceY >= 46) bounceDy = -bounceDy;

        oled.fillRoundRect(bounceX, bounceY, 44, 16, 4, SSD1306_WHITE);
        oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(bounceX + 6, bounceY + 4);
        oled.print("CHAOS");

        animFrame++;
    }

    // --- OLED Screensaver 4: 3D Tunnel Zoom (128x64) ---
    void renderOled3DTunnel(Adafruit_SSD1306& oled) {
        oled.clearDisplay();
        int cx = 64;
        int cy = 32;

        int step = (animFrame * 2) % 16;
        for (int r = step; r < 60; r += 12) {
            int w = r * 2;
            int h = r;
            oled.drawRoundRect(cx - w / 2, cy - h / 2, w, h, 2, SSD1306_WHITE);
        }

        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(32, 28);
        oled.print("WARP TUNNEL");

        animFrame++;
    }

    // --- TFT Screensaver 1: 3D Cosmic Warp (128x160) ---
    void renderTftCosmicWarp(Adafruit_GFX& gfx, uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg) {
        gfx.fillScreen(colorBg);

        int centerX = 64;
        int centerY = 80;

        for (int i = 0; i < 30; i++) {
            float prevZ = stars[i].z;
            stars[i].z -= 4.0f;
            if (stars[i].z <= 2.0f) {
                resetStar(i);
                continue;
            }

            int prevX = centerX + (int)((stars[i].x / prevZ) * 40.0f);
            int prevY = centerY + (int)((stars[i].y / prevZ) * 40.0f);
            int sx = centerX + (int)((stars[i].x / stars[i].z) * 40.0f);
            int sy = centerY + (int)((stars[i].y / stars[i].z) * 40.0f);

            if (sx >= 0 && sx < 128 && sy >= 18 && sy < 146) {
                gfx.drawLine(prevX, prevY, sx, sy, colorPrimary);
            }
        }

        int px = 64;
        int py = 70;
        gfx.fillCircle(px, py, 14, colorAccent);
        gfx.drawCircle(px, py, 14, colorPrimary);
        gfx.drawFastHLine(px - 22, py, 44, colorPrimary);
        gfx.drawFastHLine(px - 18, py - 1, 36, colorAccent);

        float angle = (animFrame * 0.15f);
        int mx = px + (int)(cos(angle) * 26.0f);
        int my = py + (int)(sin(angle) * 8.0f);
        gfx.fillCircle(mx, my, 4, colorText);

        bounceX += bounceDx;
        bounceY += bounceDy;

        if (bounceX <= 4 || bounceX >= 68) bounceDx = -bounceDx;
        if (bounceY <= 22 || bounceY >= 135) bounceDy = -bounceDy;

        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(1);
        gfx.setCursor(bounceX, bounceY);
        gfx.print("CHAOS");

        animFrame++;
    }

    // --- TFT Screensaver 2: Matrix Code Rain (128x160) ---
    void renderTftMatrixRain(Adafruit_GFX& gfx, uint16_t colorPrimary, uint16_t colorBg) {
        gfx.fillScreen(colorBg);
        gfx.setTextSize(1);

        for (int i = 0; i < 10; i++) {
            int x = i * 12 + 6;
            matrixY[i] += matrixSpeed[i];
            if (matrixY[i] > 140) {
                matrixY[i] = random(-30, 0);
                matrixSpeed[i] = random(3, 6);
            }

            // Head char
            gfx.setTextColor(ST77XX_WHITE, colorBg);
            gfx.setCursor(x, matrixY[i]);
            gfx.print((char)random(33, 126));

            // Tail
            gfx.drawFastVLine(x + 3, max(18, matrixY[i] - 24), 20, colorPrimary);
        }

        gfx.drawFastHLine(0, 142, 128, colorPrimary);
        gfx.setTextColor(colorPrimary, colorBg);
        gfx.setCursor(14, 146);
        gfx.print("MATRIX CODE RAIN");
        animFrame++;
    }

    // --- TFT Screensaver 3: Synthwave Retro Horizon (128x160) ---
    void renderTftSynthwaveGrid(Adafruit_GFX& gfx, uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg) {
        gfx.fillScreen(colorBg);

        // Synthwave Sun on horizon
        int sunY = 74;
        gfx.fillCircle(64, sunY, 18, colorAccent);
        gfx.drawFastHLine(44, sunY - 4, 40, colorBg);
        gfx.drawFastHLine(42, sunY, 44, colorBg);
        gfx.drawFastHLine(44, sunY + 4, 40, colorBg);

        // Horizon Line
        gfx.drawFastHLine(0, sunY + 18, 128, colorPrimary);

        // Perspective Ground Grid Lines
        for (int i = -3; i <= 3; i++) {
            int x1 = 64 + (i * 6);
            int x2 = 64 + (i * 24);
            gfx.drawLine(x1, sunY + 18, x2, 146, colorPrimary);
        }

        // Horizontal Grid Scroll
        int gridOffset = (animFrame * 2) % 14;
        for (int y = sunY + 18 + gridOffset; y < 146; y += 14) {
            gfx.drawFastHLine(0, y, 128, colorPrimary);
        }

        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(1);
        gfx.setCursor(20, 24);
        gfx.print("SYNTHWAVE 80S");
        animFrame++;
    }

    // --- TFT Screensaver 4: DVD Bouncing Logo (128x160) ---
    void renderTftDvdLogo(Adafruit_GFX& gfx, uint16_t colorBg) {
        gfx.fillScreen(colorBg);

        bounceX += bounceDx;
        bounceY += bounceDy;

        if (bounceX <= 4 || bounceX >= 74) {
            bounceDx = -bounceDx;
            dvdColor = (dvdColor == ST77XX_CYAN) ? ST77XX_MAGENTA : (dvdColor == ST77XX_MAGENTA) ? ST77XX_YELLOW : (dvdColor == ST77XX_YELLOW) ? ST77XX_GREEN : ST77XX_CYAN;
        }
        if (bounceY <= 20 || bounceY >= 126) {
            bounceDy = -bounceDy;
            dvdColor = (dvdColor == ST77XX_CYAN) ? ST77XX_MAGENTA : (dvdColor == ST77XX_MAGENTA) ? ST77XX_YELLOW : (dvdColor == ST77XX_YELLOW) ? ST77XX_GREEN : ST77XX_CYAN;
        }

        gfx.fillRoundRect(bounceX, bounceY, 48, 20, 6, dvdColor);
        gfx.setTextColor(colorBg, dvdColor);
        gfx.setTextSize(1);
        gfx.setCursor(bounceX + 8, bounceY + 6);
        gfx.print("CHAOS");

        animFrame++;
    }
};

#endif // SCREENSAVER_H
