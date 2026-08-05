import re

with open("include/display_oled.h", "r") as f:
    content = f.read()

# Update the switch statement
old_switch = """        switch (clockStyle) {
            case 0: drawOledClock_DigitalHUD(h, m, s_val, ipStr, s); break;
            case 1: drawOledClock_AnalogMinimal(h, m, s_val, s); break;
            case 2: drawOledClock_CyberMatrix(h, m, s_val, s); break;
            case 3: drawOledClock_RetroFlip(h, m, s_val, s); break;
            case 4: drawOledClock_VerticalStack(h, m, s_val, s); break;
            case 5: drawOledClock_BinaryGauges(h, m, s_val, s); break;
            case 6: drawOledClock_CyberpunkBox(h, m, s_val, s); break;
            case 7: drawOledClock_RadialHorizon(h, m, s_val, s); break;
            default: drawOledClock_DigitalHUD(h, m, s_val, ipStr, s); break;
        }"""
new_switch = """        switch (clockStyle) {
            case 0: drawOledClock_DigitalHUD(h, m, s_val, ipStr, s); break;
            case 1: drawOledClock_AnalogMinimal(h, m, s_val, s); break;
            case 2: drawOledClock_CyberMatrix(h, m, s_val, s); break;
            case 3: drawOledClock_RetroFlip(h, m, s_val, s); break;
            case 4: drawOledClock_VerticalStack(h, m, s_val, s); break;
            case 5: drawOledClock_BinaryGauges(h, m, s_val, s); break;
            case 6: drawOledClock_CyberpunkBox(h, m, s_val, s); break;
            case 7: drawOledClock_RadialHorizon(h, m, s_val, s); break;
            case 8: drawOledClock_Orbit(h, m, s_val, s); break;
            case 9: drawOledClock_Word(h, m, s_val, s); break;
            case 10: drawOledClock_DotMatrix(h, m, s_val, s); break;
            case 11: drawOledClock_BarGraph(h, m, s_val, s); break;
            default: drawOledClock_DigitalHUD(h, m, s_val, ipStr, s); break;
        }"""
content = content.replace(old_switch, new_switch)

new_funcs = """    // --- OLED Clock Style 8: Orbit ---
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
        
        // Remove text from right side since it takes too much space
    }
};"""

content = content.replace("};", new_funcs)

with open("include/display_oled.h", "w") as f:
    f.write(content)

with open("src/main.cpp", "r") as f:
    main_content = f.read()

main_content = main_content.replace("% 8;", "% 12;")

with open("src/main.cpp", "w") as f:
    f.write(main_content)

print("Patched!")
