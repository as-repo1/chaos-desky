import re

with open("include/watchface_engine.h", "r") as f:
    c = f.read()

# First, undo the bad switch case addition
c = c.replace("""        case WATCHFACE_CYBERPUNK_2077:
            renderCyberpunk2077(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
            break;
        case WATCHFACE_CHRONOGRAPH:
            drawChronograph(h, m, s, sm);
            break;
        case WATCHFACE_BAUHAUS:
            drawBauhaus(h, m, s, sm);
            break;""", """        case WATCHFACE_CYBERPUNK_2077:
            renderCyberpunk2077(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
            break;
        case WATCHFACE_CHRONOGRAPH:
            renderChronograph(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
            break;
        case WATCHFACE_BAUHAUS:
            renderBauhaus(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
            break;""")

# Now find where my bad functions start and remove them.
bad_funcs_start = c.find("    // --- WATCH FACE: CLASSIC CHRONOGRAPH (13) ---")
if bad_funcs_start != -1:
    c = c[:bad_funcs_start]

# Define new proper functions
new_funcs = """
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
"""

c = c + new_funcs

# Wait, check if my earlier regex changed the switch case completely correctly:
# Let's ensure the switch has WATCHFACE_CYBERPUNK_2077, WATCHFACE_CHRONOGRAPH, WATCHFACE_BAUHAUS
# Oh wait, my find/replace used `drawChronograph`, I need to replace it.
if "case WATCHFACE_CYBERPUNK_2077:\n            drawCyberpunk2077(h, m, s, sm);\n            break;" in c:
    pass # Wait, that regex failed because the original was `renderCyberpunk2077(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);`
    
with open("include/watchface_engine.h", "w") as f:
    f.write(c)

print("TFT fixed!")
