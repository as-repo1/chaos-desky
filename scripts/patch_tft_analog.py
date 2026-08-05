import re

with open("include/watchface_engine.h", "r") as f:
    c = f.read()

# Add enums
c = c.replace(
    "WATCHFACE_CYBERPUNK_2077 = 12",
    "WATCHFACE_CYBERPUNK_2077 = 12,\n    WATCHFACE_CHRONOGRAPH = 13,\n    WATCHFACE_BAUHAUS = 14"
)

# Update the switch inside drawWatchFace
c = c.replace(
    "case WATCHFACE_CYBERPUNK_2077:\n            drawCyberpunk2077(h, m, s, sm);\n            break;",
    "case WATCHFACE_CYBERPUNK_2077:\n            drawCyberpunk2077(h, m, s, sm);\n            break;\n        case WATCHFACE_CHRONOGRAPH:\n            drawChronograph(h, m, s, sm);\n            break;\n        case WATCHFACE_BAUHAUS:\n            drawBauhaus(h, m, s, sm);\n            break;"
)

new_funcs = """
    // --- WATCH FACE: CLASSIC CHRONOGRAPH (13) ---
    void drawChronograph(int h, int m, int s, const SensorManager& sm) const {
        tft.fillScreen(TFT_BLACK);
        int cx = 120, cy = 120;
        int r = 110;
        
        // Outer dial
        tft.drawCircle(cx, cy, r, TFT_SILVER);
        tft.drawCircle(cx, cy, r - 2, TFT_DARKGREY);

        // Tick marks
        for (int i = 0; i < 60; i++) {
            float angle = i * 6.0f * (PI / 180.0f);
            int len = (i % 5 == 0) ? 10 : 4;
            int color = (i % 5 == 0) ? TFT_WHITE : TFT_LIGHTGREY;
            tft.drawLine(cx + cos(angle) * (r - len), cy + sin(angle) * (r - len),
                         cx + cos(angle) * r, cy + sin(angle) * r, color);
        }

        // Sub-dials
        int sdR = 20;
        // Sub-dial 1 (Left - Temp)
        tft.drawCircle(cx - 40, cy, sdR, TFT_DARKGREY);
        tft.setTextColor(TFT_LIGHTGREY);
        tft.setTextSize(1);
        tft.setCursor(cx - 52, cy + sdR + 2); tft.print("TEMP");
        float tempAngle = map(sm.data.tempC, 0, 50, 0, 360) * (PI / 180.0f) - HALF_PI;
        tft.drawLine(cx - 40, cy, cx - 40 + cos(tempAngle) * (sdR - 4), cy + sin(tempAngle) * (sdR - 4), TFT_RED);

        // Sub-dial 2 (Right - Humidity)
        tft.drawCircle(cx + 40, cy, sdR, TFT_DARKGREY);
        tft.setCursor(cx + 32, cy + sdR + 2); tft.print("HUM");
        float humAngle = map(sm.data.humidity, 0, 100, 0, 360) * (PI / 180.0f) - HALF_PI;
        tft.drawLine(cx + 40, cy, cx + 40 + cos(humAngle) * (sdR - 4), cy + sin(humAngle) * (sdR - 4), TFT_CYAN);

        // Hands
        float hAngle = (h % 12 + m / 60.0f) * 30.0f * (PI / 180.0f) - HALF_PI;
        float mAngle = (m + s / 60.0f) * 6.0f * (PI / 180.0f) - HALF_PI;
        float sAngle = s * 6.0f * (PI / 180.0f) - HALF_PI;

        // Draw hour hand
        tft.drawLine(cx, cy, cx + cos(hAngle) * 50, cy + sin(hAngle) * 50, TFT_WHITE);
        tft.drawLine(cx + 1, cy, cx + 1 + cos(hAngle) * 50, cy + sin(hAngle) * 50, TFT_WHITE);
        
        // Draw minute hand
        tft.drawLine(cx, cy, cx + cos(mAngle) * 80, cy + sin(mAngle) * 80, TFT_WHITE);
        tft.drawLine(cx + 1, cy, cx + 1 + cos(mAngle) * 80, cy + sin(mAngle) * 80, TFT_WHITE);

        // Draw second hand
        tft.drawLine(cx - cos(sAngle) * 15, cy - sin(sAngle) * 15, cx + cos(sAngle) * 90, cy + sin(sAngle) * 90, TFT_RED);
        tft.fillCircle(cx, cy, 4, TFT_RED);
    }

    // --- WATCH FACE: BAUHAUS MINIMAL (14) ---
    void drawBauhaus(int h, int m, int s, const SensorManager& sm) const {
        tft.fillScreen(0x18E3); // Very dark gray/blue
        int cx = 120, cy = 120;
        int r = 110;
        
        // Just minimal markers for 12, 3, 6, 9
        tft.fillRect(cx - 2, cy - r, 4, 15, TFT_RED);
        tft.fillRect(cx + r - 15, cy - 2, 15, 4, TFT_WHITE);
        tft.fillRect(cx - 2, cy + r - 15, 4, 15, TFT_WHITE);
        tft.fillRect(cx - r, cy - 2, 15, 4, TFT_WHITE);

        float hAngle = (h % 12 + m / 60.0f) * 30.0f * (PI / 180.0f) - HALF_PI;
        float mAngle = (m + s / 60.0f) * 6.0f * (PI / 180.0f) - HALF_PI;
        float sAngle = s * 6.0f * (PI / 180.0f) - HALF_PI;

        // Hour hand (thick, blocky)
        float hx = cos(hAngle);
        float hy = sin(hAngle);
        for(int w = -3; w <= 3; w++) {
            tft.drawLine(cx + hy * w, cy - hx * w, cx + hx * 60 + hy * w, cy + hy * 60 - hx * w, TFT_WHITE);
        }

        // Minute hand (thinner, longer)
        float mx = cos(mAngle);
        float my = sin(mAngle);
        for(int w = -1; w <= 1; w++) {
            tft.drawLine(cx + my * w, cy - mx * w, cx + mx * 95 + my * w, cy + my * 95 - mx * w, TFT_WHITE);
        }

        // Second hand (red, very thin)
        tft.drawLine(cx, cy, cx + cos(sAngle) * 105, cy + sin(sAngle) * 105, TFT_RED);
        
        // Center cap
        tft.fillCircle(cx, cy, 6, TFT_WHITE);
        tft.fillCircle(cx, cy, 2, TFT_BLACK);
    }
"""

idx = c.rfind("};")
c = c[:idx] + new_funcs + c[idx:]

with open("include/watchface_engine.h", "w") as f:
    f.write(c)

print("TFT updated")
