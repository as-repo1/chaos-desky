import re

with open("include/display_oled.h", "r") as f:
    c = f.read()

# 1. Update the signature of draw()
c = c.replace(
    "void draw(SensorManager& sm, const NotificationManager& notifMgr, const String& ipStr, const String& timeStr, int rssi, int clockStyle = 0)",
    "void draw(SensorManager& sm, const NotificationManager& notifMgr, const String& ipStr, int h, int m, int s_val, int rssi, int clockStyle = 0)"
)
c = c.replace("timeStr != lastTimeStr", "(h != lastH || m != lastM || s_val != lastS)")
c = c.replace("lastTimeStr = timeStr;", "lastH = h; lastM = m; lastS = s_val;")
c = c.replace("String lastTimeStr = \"\";", "int lastH = -1, lastM = -1, lastS = -1;")

# Update drawMode0_HUD and drawMode1_BigClock calls inside draw()
c = c.replace("drawMode0_HUD(sm.data, ipStr, timeStr, rssi);", "drawMode0_HUD(sm.data, ipStr, h, m, s_val, rssi);")
c = c.replace("drawMode1_BigClock(timeStr, ipStr, sm.data, clockStyle);", "drawMode1_BigClock(h, m, s_val, ipStr, sm.data, clockStyle);")

# Update drawMode0_HUD signature
c = c.replace("void drawMode0_HUD(const SensorData& s, const String& ipStr, const String& timeStr, int rssi) {", "void drawMode0_HUD(const SensorData& s, const String& ipStr, int h, int m, int s_val, int rssi) {")
c = c.replace("oled.print(timeStr);", "oled.printf(\"%02d:%02d:%02d\", h, m, s_val);")

# Update drawMode1_BigClock signature
c = c.replace("void drawMode1_BigClock(const String& timeStr, const String& ipStr, const SensorData& s, int clockStyle = 0) {", "void drawMode1_BigClock(int h, int m, int s_val, const String& ipStr, const SensorData& s, int clockStyle = 0) {")

# Update the switch inside drawMode1_BigClock
c = re.sub(r'drawOledClock_([A-Za-z0-9_]+)\(timeStr', r'drawOledClock_\1(h, m, s_val', c)

# Update all drawOledClock_* signatures
c = re.sub(r'void drawOledClock_([A-Za-z0-9_]+)\(const String& timeStr', r'void drawOledClock_\1(int h, int m, int s_val', c)

# Replace all timeStr substring usages inside drawOledClock_*
c = c.replace("oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : \"00:00\");", "oled.printf(\"%02d:%02d\", h, m);")
c = c.replace("oled.print(timeStr.length() >= 8 ? timeStr.substring(6, 8) : \"00\");", "oled.printf(\"%02d\", s_val);")
c = c.replace("oled.print(timeStr.length() >= 2 ? timeStr.substring(0, 2) : \"00\");", "oled.printf(\"%02d\", h);")
c = c.replace("oled.print(timeStr.length() >= 5 ? timeStr.substring(3, 5) : \"00\");", "oled.printf(\"%02d\", m);")
c = c.replace("oled.print(timeStr.length() >= 8 ? timeStr : \"00:00:00\");", "oled.printf(\"%02d:%02d:%02d\", h, m, s_val);")
c = c.replace("int sec = timeStr.length() >= 8 ? timeStr.substring(6, 8).toInt() : 0;", "int sec = s_val;")
c = re.sub(r'int h = 0, m = 0, sec = 0;\s*if \(timeStr.*?\}', 'int sec = s_val;', c, flags=re.DOTALL)
c = c.replace("int digits[6] = {", "const int digits[6] = {")

# Add the 4 new styles to the switch
c = c.replace(
    "case 7: drawOledClock_RadialHorizon(h, m, s_val, s); break;",
    "case 7: drawOledClock_RadialHorizon(h, m, s_val, s); break;\n            case 8: drawOledClock_Orbit(h, m, s_val, s); break;\n            case 9: drawOledClock_Word(h, m, s_val, s); break;\n            case 10: drawOledClock_DotMatrix(h, m, s_val, s); break;\n            case 11: drawOledClock_BarGraph(h, m, s_val, s); break;"
)

# Insert the 4 new functions right before the final `};` of OledDisplayManager.
idx = c.rfind("};")

new_funcs = """
    // --- OLED Clock Style 8: Orbit ---
    void drawOledClock_Orbit(int h, int m, int s_val, const SensorData& s) {
        int cx = 64, cy = 32;
        oled.drawCircle(cx, cy, 30, SSD1306_WHITE);
        oled.drawCircle(cx, cy, 20, SSD1306_WHITE);
        oled.drawCircle(cx, cy, 10, SSD1306_WHITE);

        float hAngle = (h % 12 + m / 60.0f) * 30.0f * (PI / 180.0f) - HALF_PI;
        float mAngle = m * 6.0f * (PI / 180.0f) - HALF_PI;
        float sAngle = s_val * 6.0f * (PI / 180.0f) - HALF_PI;

        oled.fillCircle(cx + cos(hAngle) * 10, cy + sin(hAngle) * 10, 3, SSD1306_WHITE);
        oled.fillCircle(cx + cos(mAngle) * 20, cy + sin(mAngle) * 20, 3, SSD1306_WHITE);
        oled.fillCircle(cx + cos(sAngle) * 30, cy + sin(sAngle) * 30, 2, SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(0, 0); oled.printf("%02d:%02d", h, m);
    }

    // --- OLED Clock Style 9: Typographic Word ---
    void drawOledClock_Word(int h, int m, int s_val, const SensorData& s) {
        const char* ones[] = {"ZERO", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN", "TWELVE", "THIRTEEN", "FOURTEEN", "FIFTEEN", "SIXTEEN", "SEVENTEEN", "EIGHTEEN", "NINETEEN"};
        const char* tens[] = {"", "", "TWENTY", "THIRTY", "FORTY", "FIFTY"};

        String hStr = (h % 12 == 0) ? "TWELVE" : (h % 12 <= 19 ? ones[h % 12] : "??");
        String mStr = "O'CLOCK";
        if (m > 0) {
            if (m < 20) mStr = ones[m];
            else if (m % 10 == 0) mStr = tens[m / 10];
            else mStr = String(tens[m / 10]) + " " + ones[m % 10];
        }

        oled.setTextSize(2);
        oled.setTextColor(SSD1306_WHITE);
        oled.setCursor(4, 8);
        oled.print(hStr);
        oled.setCursor(4, 32);
        oled.print(mStr);
        oled.setTextSize(1);
        oled.setCursor(4, 54);
        oled.printf("%s %02d SEC", (h >= 12) ? "PM" : "AM", s_val);
    }

    // --- OLED Clock Style 10: Dot Matrix ---
    void drawOledClock_DotMatrix(int h, int m, int s_val, const SensorData& s) {
        int vals[3] = {h, m, s_val};
        for (int row = 0; row < 3; row++) {
            for (int bit = 5; bit >= 0; bit--) {
                int x = 20 + (5 - bit) * 16;
                int y = 8 + row * 18;
                if (vals[row] & (1 << bit)) {
                    oled.fillRect(x, y, 10, 10, SSD1306_WHITE);
                } else {
                    oled.drawRect(x, y, 10, 10, SSD1306_WHITE);
                }
            }
        }
        oled.setTextSize(1);
        oled.setCursor(2, 10); oled.print("H");
        oled.setCursor(2, 28); oled.print("M");
        oled.setCursor(2, 46); oled.print("S");
    }

    // --- OLED Clock Style 11: Bar Graph ---
    void drawOledClock_BarGraph(int h, int m, int s_val, const SensorData& s) {
        oled.drawRect(16, 4, 100, 12, SSD1306_WHITE);
        oled.drawRect(16, 26, 100, 12, SSD1306_WHITE);
        oled.drawRect(16, 48, 100, 12, SSD1306_WHITE);

        int hFill = (h * 96) / 24;
        int mFill = (m * 96) / 60;
        int sFill = (s_val * 96) / 60;

        oled.fillRect(18, 6, hFill, 8, SSD1306_WHITE);
        oled.fillRect(18, 28, mFill, 8, SSD1306_WHITE);
        oled.fillRect(18, 50, sFill, 8, SSD1306_WHITE);

        oled.setTextSize(1);
        oled.setCursor(4, 6); oled.print("H");
        oled.setCursor(4, 28); oled.print("M");
        oled.setCursor(4, 50); oled.print("S");
    }
"""

c = c[:idx] + new_funcs + c[idx:]

with open("include/display_oled.h", "w") as f:
    f.write(c)

print("Applied!")
