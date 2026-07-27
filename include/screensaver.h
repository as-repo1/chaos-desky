#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

class ScreensaverEngine {
public:
    int animFrame = 0;
    int bounceX = 10;
    int bounceY = 30;
    int bounceDx = 2;
    int bounceDy = 2;
    int oledSaverMode = 0;  // 0: Matrix, 1: DVD, 2: Tunnel, 3: DNA Helix, 4: Batman Signal, 5: Linux Tux

    // Matrix Rain State
    int matrixY[10];
    int matrixSpeed[10];

    ScreensaverEngine() {
        for (int i = 0; i < 10; i++) {
            matrixY[i] = random(-40, 0);
            matrixSpeed[i] = random(2, 5);
        }
    }

    void renderOledScreensaver(Adafruit_SSD1306& oled) {
        if (animFrame % 250 == 0 && animFrame > 0) {
            oledSaverMode = (oledSaverMode + 1) % 6;
        }

        switch (oledSaverMode) {
            case 0: renderOledMatrixRain(oled); break;
            case 1: renderOledDvdBounce(oled); break;
            case 2: renderOled3DTunnel(oled); break;
            case 3: renderOledDnaHelix(oled); break;
            case 4: renderOledBatmanSignal(oled); break;
            case 5: renderOledLinuxTux(oled); break;
            default: renderOledMatrixRain(oled); break;
        }
    }

private:
    // --- OLED Screensaver 0: Matrix Code Rain ---
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

            oled.setCursor(x, matrixY[i]);
            char c = (char)random(33, 126);
            oled.print(c);
            oled.drawFastVLine(x + 3, max(0, matrixY[i] - 16), 12, SSD1306_WHITE);
        }

        oled.drawFastHLine(0, 58, 128, SSD1306_WHITE);
        oled.setCursor(16, 52);
        oled.print("MATRIX CODE RAIN");
        animFrame++;
    }

    // --- OLED Screensaver 1: DVD Bouncing Logo ---
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

    // --- OLED Screensaver 2: 3D Tunnel Zoom ---
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

    // --- OLED Screensaver 3: DNA Double Helix ---
    void renderOledDnaHelix(Adafruit_SSD1306& oled) {
        oled.clearDisplay();
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(22, 2);
        oled.print("DNA DOUBLE HELIX");
        oled.drawFastHLine(0, 12, 128, SSD1306_WHITE);

        float phase = animFrame * 0.15f;
        for (int x = 10; x < 118; x += 6) {
            float angle = phase + (x * 0.08f);
            int y1 = 36 + (int)(sin(angle) * 16.0f);
            int y2 = 36 - (int)(sin(angle) * 16.0f);

            // Connect rungs every 12 pixels when strings are apart
            if (x % 12 == 0) {
                oled.drawLine(x, y1, x, y2, SSD1306_WHITE);
            }
            oled.fillCircle(x, y1, 2, SSD1306_WHITE);
            oled.drawCircle(x, y2, 2, SSD1306_WHITE);
        }
        animFrame++;
    }

    // --- OLED Screensaver 4: Batman Signal Searchlight ---
    void renderOledBatmanSignal(Adafruit_SSD1306& oled) {
        oled.clearDisplay();
        int cx = 64, cy = 28;

        // Pulsing searchlight oval beam
        int radiusX = 46 + (int)(sin(animFrame * 0.12f) * 4.0f);
        int radiusY = 22 + (int)(sin(animFrame * 0.12f) * 2.0f);
        oled.fillRoundRect(cx - radiusX, cy - radiusY, radiusX * 2, radiusY * 2, radiusY, SSD1306_WHITE);

        // Bat symbol cutout (Black inside white beam)
        oled.fillRoundRect(cx - 30, cy - 8, 60, 16, 6, SSD1306_BLACK);
        
        // Bat wings scalloped cutouts from bottom
        oled.fillCircle(cx - 16, cy + 10, 8, SSD1306_WHITE);
        oled.fillCircle(cx, cy + 12, 7, SSD1306_WHITE);
        oled.fillCircle(cx + 16, cy + 10, 8, SSD1306_WHITE);

        // Bat head and ears
        oled.fillTriangle(cx - 5, cy - 8, cx - 4, cy - 14, cx - 1, cy - 8, SSD1306_BLACK);
        oled.fillTriangle(cx + 5, cy - 8, cx + 4, cy - 14, cx + 1, cy - 8, SSD1306_BLACK);
        oled.fillCircle(cx, cy - 8, 4, SSD1306_BLACK);
        // Cutout space between ears
        oled.fillTriangle(cx - 2, cy - 14, cx + 2, cy - 14, cx, cy - 9, SSD1306_WHITE);

        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(18, 55);
        oled.print("~ THE DARK KNIGHT ~");
        animFrame++;
    }

    // --- OLED Screensaver 5: Linux Tux Mascot ---
    void renderOledLinuxTux(Adafruit_SSD1306& oled) {
        oled.clearDisplay();
        int cx = 64, cy = 28;

        // Penguin silhouette (Body & Head)
        oled.fillRoundRect(cx - 14, cy - 16, 28, 36, 12, SSD1306_WHITE);
        oled.fillRoundRect(cx - 10, cy - 2, 20, 20, 8, SSD1306_BLACK); // White belly in inverted contrast

        // Flippers (Wiggling up and down)
        int flipperOffset = (animFrame % 4 < 2) ? -2 : 2;
        oled.fillTriangle(cx - 14, cy, cx - 24, cy + 10 + flipperOffset, cx - 12, cy + 14, SSD1306_WHITE);
        oled.fillTriangle(cx + 14, cy, cx + 24, cy + 10 - flipperOffset, cx + 12, cy + 14, SSD1306_WHITE);

        // Feet (Webbed triangles at base)
        oled.fillTriangle(cx - 10, cy + 18, cx - 16, cy + 23, cx - 4, cy + 23, SSD1306_WHITE);
        oled.fillTriangle(cx + 10, cy + 18, cx + 16, cy + 23, cx + 4, cy + 23, SSD1306_WHITE);

        // Eyes (Blinking animation)
        if ((animFrame % 30) > 2) {
            oled.fillCircle(cx - 5, cy - 8, 3, SSD1306_BLACK);
            oled.fillCircle(cx + 5, cy - 8, 3, SSD1306_BLACK);
            oled.fillCircle(cx - 4, cy - 8, 1, SSD1306_WHITE);
            oled.fillCircle(cx + 4, cy - 8, 1, SSD1306_WHITE);
        } else {
            oled.drawFastHLine(cx - 8, cy - 8, 6, SSD1306_BLACK);
            oled.drawFastHLine(cx + 2, cy - 8, 6, SSD1306_BLACK);
        }

        // Beak (Triangle under eyes)
        oled.fillTriangle(cx - 4, cy - 4, cx + 4, cy - 4, cx, cy, SSD1306_BLACK);
        oled.drawTriangle(cx - 4, cy - 4, cx + 4, cy - 4, cx, cy, SSD1306_WHITE);

        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(28, 54);
        oled.print("LINUX INSIDE");
        animFrame++;
    }
};

#endif // SCREENSAVER_H

