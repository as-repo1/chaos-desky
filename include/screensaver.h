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

class ScreensaverEngine {
public:
    int animFrame = 0;
    int bounceX = 10;
    int bounceY = 30;
    int bounceDx = 2;
    int bounceDy = 2;

    Star3D stars[20];
    Particle2D hearts[4];

    ScreensaverEngine() {
        for (int i = 0; i < 20; i++) {
            resetStar(i);
        }
        for (int i = 0; i < 4; i++) {
            hearts[i].x = 20 + (i * 25);
            hearts[i].y = 50 + (i * 12);
            hearts[i].speedY = 1 + (i % 2);
        }
    }

    void resetStar(int i) {
        stars[i].x = random(-60, 60);
        stars[i].y = random(-70, 70);
        stars[i].z = random(20, 100);
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
            // Draw small pixel heart
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
            oled.fillCircle(cx - 10, cy - 3, 2, SSD1306_WHITE); // pupil
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

    // --- Render TFT 3D Warp Starfield & Orbiting Planet (128x160) ---
    void renderTftCosmicWarp(Adafruit_GFX& gfx, uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg) {
        gfx.fillScreen(colorBg);

        // 1. Draw 3D Warp Starfield
        int centerX = 64;
        int centerY = 80;

        for (int i = 0; i < 20; i++) {
            stars[i].z -= 4.0f;
            if (stars[i].z <= 2.0f) {
                resetStar(i);
            }

            int sx = centerX + (int)((stars[i].x / stars[i].z) * 40.0f);
            int sy = centerY + (int)((stars[i].y / stars[i].z) * 40.0f);

            if (sx >= 0 && sx < 128 && sy >= 18 && sy < 146) {
                gfx.drawPixel(sx, sy, colorPrimary);
            }
        }

        // 2. Draw Orbiting Planet with Ring
        int px = 64;
        int py = 70;
        gfx.fillCircle(px, py, 14, colorAccent);
        gfx.drawCircle(px, py, 14, colorPrimary);
        gfx.drawFastHLine(px - 22, py, 44, colorPrimary); // Ring

        // 3. Orbiting Moon
        float angle = (animFrame * 0.15f);
        int mx = px + (int)(cos(angle) * 26.0f);
        int my = py + (int)(sin(angle) * 8.0f);
        gfx.fillCircle(mx, my, 4, colorText);

        // 4. Bouncing "CHAOS DESKY" Logo
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
};

#endif // SCREENSAVER_H
