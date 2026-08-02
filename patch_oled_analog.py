import re

with open("include/display_oled.h", "r") as f:
    c = f.read()

# Add cases to the switch
c = c.replace(
    "case 11: drawOledClock_BarGraph(h, m, s_val, s); break;",
    "case 11: drawOledClock_BarGraph(h, m, s_val, s); break;\n            case 12: drawOledClock_RadarSweep(h, m, s_val, s); break;\n            case 13: drawOledClock_VintagePocket(h, m, s_val, s); break;"
)

new_funcs = """
    // --- OLED Clock Style 12: Radar Sweep ---
    void drawOledClock_RadarSweep(int h, int m, int s_val, const SensorData& s) {
        int cx = 64, cy = 32, r = 30;
        oled.drawCircle(cx, cy, r, SSD1306_WHITE);
        oled.drawCircle(cx, cy, r - 10, SSD1306_WHITE);
        oled.drawCircle(cx, cy, r - 20, SSD1306_WHITE);

        // Draw crosshairs
        oled.drawFastHLine(cx - r, cy, r * 2, SSD1306_WHITE);
        oled.drawFastVLine(cx, cy - r, r * 2, SSD1306_WHITE);

        float hAngle = (h % 12 + m / 60.0f) * 30.0f * (PI / 180.0f) - HALF_PI;
        float mAngle = m * 6.0f * (PI / 180.0f) - HALF_PI;
        float sAngle = s_val * 6.0f * (PI / 180.0f) - HALF_PI;

        // Draw hours and minutes as blips
        oled.fillCircle(cx + cos(hAngle) * (r - 15), cy + sin(hAngle) * (r - 15), 3, SSD1306_WHITE);
        oled.fillCircle(cx + cos(mAngle) * (r - 5), cy + sin(mAngle) * (r - 5), 2, SSD1306_WHITE);

        // Draw sweeping radar line for seconds
        oled.drawLine(cx, cy, cx + cos(sAngle) * r, cy + sin(sAngle) * r, SSD1306_WHITE);
    }

    // --- OLED Clock Style 13: Vintage Pocket Watch ---
    void drawOledClock_VintagePocket(int h, int m, int s_val, const SensorData& s) {
        int cx = 64, cy = 32, r = 30;
        oled.drawCircle(cx, cy, r, SSD1306_WHITE);
        oled.drawCircle(cx, cy, r - 2, SSD1306_WHITE);

        // Minimal Roman numerals logic (just text at compass points)
        oled.setTextSize(1);
        oled.setCursor(cx - 5, cy - r + 4); oled.print("XII");
        oled.setCursor(cx - 3, cy + r - 10); oled.print("VI");
        oled.setCursor(cx + r - 16, cy - 3); oled.print("III");
        oled.setCursor(cx - r + 4, cy - 3); oled.print("IX");

        float hAngle = (h % 12 + m / 60.0f) * 30.0f * (PI / 180.0f) - HALF_PI;
        float mAngle = m * 6.0f * (PI / 180.0f) - HALF_PI;
        
        // Hour hand
        oled.drawLine(cx, cy, cx + cos(hAngle) * 14, cy + sin(hAngle) * 14, SSD1306_WHITE);
        // Minute hand
        oled.drawLine(cx, cy, cx + cos(mAngle) * 22, cy + sin(mAngle) * 22, SSD1306_WHITE);
        // Second hand (tiny dot at edge to simulate mechanical movement)
        float sAngle = s_val * 6.0f * (PI / 180.0f) - HALF_PI;
        oled.drawPixel(cx + cos(sAngle) * 25, cy + sin(sAngle) * 25, SSD1306_WHITE);
    }
"""

idx = c.rfind("};")
c = c[:idx] + new_funcs + c[idx:]

with open("include/display_oled.h", "w") as f:
    f.write(c)

print("OLED updated")
