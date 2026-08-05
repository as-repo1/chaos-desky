#include "display_oled.h"

bool OledDisplayManager::begin() {
    if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
        Serial.println("❌ OLED SSD1306 Allocation Failed!");
        return false;
    }

    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(15, 20);
    oled.println("CHAOS DESKY v1.0");
    oled.setCursor(20, 35);
    oled.println("Initializing...");
    oled.display();
    return true;
}

void OledDisplayManager::setContrast(uint8_t val) {
    contrast = val;
    oled.ssd1306_command(SSD1306_SETCONTRAST);
    oled.ssd1306_command(val);
}

void OledDisplayManager::setInverted(bool invert) {
    isInverted = invert;
    oled.invertDisplay(invert);
}

void OledDisplayManager::draw(SensorManager& sm, const NotificationManager& notifMgr, const String& ipStr, int h, int m, int s_val, int rssi, int clockStyle) {
    bool needsRefresh = false;
    if (notifMgr.isOledActive() || oledMode == 3 || oledMode == 4 || oledMode == 5 || (h != lastH || m != lastM || s_val != lastS) || oledMode != lastRenderedMode || clockStyle != lastClockStyle || abs(sm.data.tempC - lastTemp) > 0.1f) {
        needsRefresh = true;
    }

    if (!needsRefresh) {
        return; 
    }

    lastH = h; lastM = m; lastS = s_val;
    lastRenderedMode = oledMode;
    lastClockStyle = clockStyle;
    lastTemp = sm.data.tempC;

    oled.clearDisplay();

    if (notifMgr.isOledActive()) {
        drawNotificationOverlay(notifMgr.currentNotif, notifMgr.getProgress());
    } else {
        switch (oledMode) {
            case 0: drawMode0_HUD(sm.data, ipStr, h, m, s_val, rssi); break;
            case 1: drawMode1_BigClock(h, m, s_val, ipStr, sm.data, clockStyle); break;
            case 2: drawMode2_Sparklines(sm); break;
            case 3: drawMode3_Marquee(customText); break;
            case 4: screensaverEngine.renderOledScreensaver(oled); break;
            case 5: drawMode5_WifiInfo(ipStr, AP_SSID, AP_PASS); break;
            case 6: drawMode6_Climate(sm.data); break;
            default: drawMode0_HUD(sm.data, ipStr, h, m, s_val, rssi); break;
        }
    }

    oled.display();
}

void OledDisplayManager::drawNotificationOverlay(const NotificationItem& n, float progress) {
    oled.fillRoundRect(0, 0, 128, 14, 3, SSD1306_WHITE);
    oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(4, 3);

    switch (n.category) {
        case NOTIF_INFO:    oled.print("INFO ALERT"); break;
        case NOTIF_MESSAGE: oled.print("NEW MESSAGE"); break;
        case NOTIF_CALL:    oled.print("INCOMING CALL"); break;
        case NOTIF_WARNING: oled.print("WARNING!"); break;
        case NOTIF_ALERT:   oled.print("CRITICAL ALERT"); break;
    }

    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(4, 18);
    oled.print(n.title.substring(0, 20));

    oled.setCursor(4, 32);
    oled.print(n.message.substring(0, 20));

    if (n.message.length() > 20) {
        oled.setCursor(4, 44);
        oled.print(n.message.substring(20, 40));
    }
    oled.drawRoundRect(0, 56, 128, 8, 3, SSD1306_WHITE);
    int fillW = (int)((1.0f - progress) * 124.0f);
    if (fillW > 0) {
        oled.fillRoundRect(2, 58, fillW, 4, 2, SSD1306_WHITE);
    }
}

void OledDisplayManager::drawMode6_Climate(const SensorData& s) {
    oled.setTextSize(2);
    oled.setCursor(0, 10);
    oled.printf("%.1f C", s.tempC);
    
    oled.setCursor(0, 36);
    oled.printf("%.0f %%", s.humidity);
    
    oled.setTextSize(1);
    
    oled.drawFastHLine(0, 31, 60, SSD1306_WHITE);
    oled.drawFastVLine(78, 0, 64, SSD1306_WHITE);
    
    oled.setCursor(84, 8);
    oled.print("TEMP");
    
    oled.setCursor(84, 34);
    oled.print("HUMID");
    
    oled.setCursor(84, 52);
    oled.print("hPa");
    oled.setCursor(84, 42);
    oled.printf("%.0f", s.pressureHpa);
}

void OledDisplayManager::drawMode0_HUD(const SensorData& s, const String& ipStr, int h, int m, int s_val, int rssi) {
    oled.drawFastHLine(0, 12, 128, SSD1306_WHITE);
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(2, 2);
    oled.printf("%02d:%02d", h, m);

    oled.setCursor(44, 2);
    oled.print(WiFi.status() == WL_CONNECTED ? "ONLINE" : "OFFLINE");

    oled.setCursor(96, 2);
    oled.printf("%ddB", rssi);

    oled.drawFastVLine(64, 13, 38, SSD1306_WHITE);

    oled.setCursor(2, 16);
    oled.printf("TMP:%.1fC", s.tempC);
    oled.setCursor(68, 16);
    oled.printf("HUM:%.0f%%", s.humidity);

    oled.setCursor(2, 28);
    oled.printf("PRS:%.0fh", s.pressureHpa);
    oled.setCursor(68, 28);
    oled.printf("ALT:%.0fm", s.altitudeM);

    oled.drawFastHLine(0, 39, 128, SSD1306_WHITE);

    oled.setCursor(2, 42);
    oled.printf("HI:%.1fC   DEW:%.1fC", s.heatIndexC, s.dewPointC);
    oled.setCursor(2, 54);
    oled.printf("IP: %s", ipStr.c_str());
}

void OledDisplayManager::drawMode1_BigClock(int h, int m, int s_val, const String& ipStr, const SensorData& s, int clockStyle) {
    switch (clockStyle) {
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
        case 12: drawOledClock_RadarSweep(h, m, s_val, s); break;
        case 13: drawOledClock_VintagePocket(h, m, s_val, s); break;
        default: drawOledClock_DigitalHUD(h, m, s_val, ipStr, s); break;
    }
}

void OledDisplayManager::drawOledClock_DigitalHUD(int h, int m, int s_val, const String& ipStr, const SensorData& s) {
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(2, 2);
    oled.print("DESK CLOCK");
    oled.setCursor(76, 2);
    oled.printf("TMP:%.1fC", s.tempC);
    oled.drawFastHLine(0, 12, 128, SSD1306_WHITE);

    oled.setTextSize(3);
    oled.setCursor(8, 18);
    oled.printf("%02d:%02d", h, m);

    oled.setTextSize(1);
    oled.setCursor(100, 32);
    oled.printf("%02d", s_val);

    oled.drawFastHLine(0, 48, 128, SSD1306_WHITE);
    oled.setCursor(4, 53);
    oled.printf("IP: %s", ipStr.c_str());
}

void OledDisplayManager::drawOledClock_AnalogMinimal(int h, int m, int s_val, const SensorData& s) {
    int cx = 32, cy = 32, r = 26;
    oled.drawCircle(cx, cy, r, SSD1306_WHITE);
    oled.fillCircle(cx, cy, 2, SSD1306_WHITE);

    oled.drawFastVLine(32, 8, 4, SSD1306_WHITE);
    oled.drawFastVLine(32, 52, 4, SSD1306_WHITE);
    oled.drawFastHLine(8, 32, 4, SSD1306_WHITE);
    oled.drawFastHLine(52, 32, 4, SSD1306_WHITE);

    int sec = s_val;
    float angleH = ((h % 12) + m / 60.0f) * (2.0f * M_PI / 12.0f) - (M_PI / 2.0f);
    int hx = cx + (int)(cos(angleH) * 14);
    int hy = cy + (int)(sin(angleH) * 14);
    oled.drawLine(cx, cy, hx, hy, SSD1306_WHITE);

    float angleM = (m + sec / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
    int mx = cx + (int)(cos(angleM) * 20);
    int my = cy + (int)(sin(angleM) * 20);
    oled.drawLine(cx, cy, mx, my, SSD1306_WHITE);

    float angleS = sec * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
    int sx = cx + (int)(cos(angleS) * 23);
    int sy = cy + (int)(sin(angleS) * 23);
    oled.drawLine(cx, cy, sx, sy, SSD1306_WHITE);

    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(2);
    oled.setCursor(64, 10);
    oled.printf("%02d:%02d", h, m);

    oled.setTextSize(1);
    oled.setCursor(66, 32);
    oled.printf("SEC : %02d", sec);
    oled.setCursor(66, 46);
    oled.printf("TEMP:%.1fC", s.tempC);
}

void OledDisplayManager::drawOledClock_CyberMatrix(int h, int m, int s_val, const SensorData& s) {
    oled.drawFastHLine(0, 0, 12, SSD1306_WHITE);
    oled.drawFastVLine(0, 0, 12, SSD1306_WHITE);
    oled.drawFastHLine(116, 0, 12, SSD1306_WHITE);
    oled.drawFastVLine(127, 0, 12, SSD1306_WHITE);
    oled.drawFastHLine(0, 63, 12, SSD1306_WHITE);
    oled.drawFastVLine(0, 52, 12, SSD1306_WHITE);
    oled.drawFastHLine(116, 63, 12, SSD1306_WHITE);
    oled.drawFastVLine(127, 52, 12, SSD1306_WHITE);

    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(18, 4);
    oled.print("[ CYBER MATRIX ]");

    oled.setTextSize(3);
    oled.setCursor(14, 20);
    oled.printf("%02d:%02d", h, m);

    oled.drawRect(102, 22, 20, 18, SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(106, 26);
    oled.printf("%02d", s_val);

    oled.setCursor(12, 48);
    oled.printf("TMP:%.1fC  HUM:%.0f%%", s.tempC, s.humidity);
}

void OledDisplayManager::drawOledClock_RetroFlip(int h, int m, int s_val, const SensorData& s) {
    oled.drawRoundRect(4, 4, 56, 44, 5, SSD1306_WHITE);
    oled.drawFastHLine(4, 26, 56, SSD1306_WHITE);

    oled.drawRoundRect(68, 4, 56, 44, 5, SSD1306_WHITE);
    oled.drawFastHLine(68, 26, 56, SSD1306_WHITE);

    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(3);
    oled.setCursor(14, 15);
    oled.printf("%02d", h);

    oled.setCursor(78, 15);
    oled.printf("%02d", m);

    int sec = s_val;
    if (sec % 2 == 0) {
        oled.fillCircle(64, 18, 2, SSD1306_WHITE);
        oled.fillCircle(64, 34, 2, SSD1306_WHITE);
    }

    oled.drawRoundRect(8, 52, 112, 12, 3, SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(14, 54);
    oled.printf("SEC: %02d  |  %.1fC", sec, s.tempC);
}

void OledDisplayManager::drawOledClock_VerticalStack(int h, int m, int s_val, const SensorData& s) {
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(3);
    oled.setCursor(10, 4);
    oled.printf("%02d", h);

    oled.setCursor(10, 34);
    oled.printf("%02d", m);

    oled.drawFastVLine(62, 0, 64, SSD1306_WHITE);

    int sec = s_val;
    oled.setTextSize(1);
    oled.setCursor(68, 4);
    oled.print("DESKY");

    oled.setTextSize(2);
    oled.setCursor(68, 18);
    oled.printf(":%02d", sec);

    oled.setTextSize(1);
    oled.setCursor(68, 40);
    oled.printf("T:%.1fC", s.tempC);
    oled.setCursor(68, 52);
    oled.printf("H:%.0f%%", s.humidity);
}

void OledDisplayManager::drawOledClock_BinaryGauges(int h, int m, int s_val, const SensorData& s) {
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(2);
    oled.setCursor(16, 2);
    oled.printf("%02d:%02d:%02d", h, m, s_val);

    oled.drawFastHLine(0, 22, 128, SSD1306_WHITE);

    int sec = s_val;
    const int digits[6] = { h / 10, h % 10, m / 10, m % 10, sec / 10, sec % 10 };
    for (int i = 0; i < 6; i++) {
        int x = 6 + i * 20;
        oled.drawRect(x, 26, 16, 36, SSD1306_WHITE);
        int fillH = (int)((digits[i] / 9.0f) * 32.0f);
        if (fillH > 0) {
            oled.fillRect(x + 2, 28 + (32 - fillH), 12, fillH, SSD1306_WHITE);
        }
    }
}

void OledDisplayManager::drawOledClock_CyberpunkBox(int h, int m, int s_val, const SensorData& s) {
    oled.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    oled.drawRect(2, 2, 124, 60, SSD1306_WHITE);
    oled.fillRect(8, 0, 48, 5, SSD1306_WHITE);
    oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(10, 0);
    oled.print("CYBER");
    
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(3);
    oled.setCursor(14, 16);
    oled.printf("%02d:%02d", h, m);
    
    int sec = s_val;
    oled.setTextSize(1);
    oled.setCursor(102, 18);
    oled.printf("%02d", sec);
    
    oled.drawFastHLine(8, 44, 112, SSD1306_WHITE);
    oled.setCursor(12, 48);
    oled.printf("TEMP: %.1fC  ALT:%dm", s.tempC, (int)s.altitudeM);
}

void OledDisplayManager::drawOledClock_RadialHorizon(int h, int m, int s_val, const SensorData& s) {
    oled.drawCircle(64, 88, 60, SSD1306_WHITE);
    oled.drawCircle(64, 88, 62, SSD1306_WHITE);
    
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(10, 4);
    oled.print("=== RADIAL HORIZON ===");
    
    oled.setTextSize(3);
    oled.setCursor(20, 20);
    oled.printf("%02d:%02d", h, m);
    
    oled.setTextSize(1);
    oled.setCursor(14, 50);
    oled.printf("HUM:%.0f%% | %.1fhPa", s.humidity, s.pressureHpa);
}

void OledDisplayManager::drawMode2_Sparklines(SensorManager& sm) {
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);

    oled.setCursor(0, 0);
    oled.printf("PRESS: %.1fhPa", sm.data.pressureHpa);
    oled.drawRoundRect(0, 12, 128, 20, 3, SSD1306_WHITE);

    if (sm.historyCount > 1) {
        float minP = 1200.0f, maxP = 0.0f;
        for (int i = 0; i < sm.historyCount; i++) {
            if (sm.pressureHistory[i] < minP) minP = sm.pressureHistory[i];
            if (sm.pressureHistory[i] > maxP) maxP = sm.pressureHistory[i];
        }
        if (maxP - minP < 1.0f) { maxP += 0.5f; minP -= 0.5f; }

        int step = 124 / (sm.historyCount - 1);
        for (int i = 0; i < sm.historyCount - 1; i++) {
            int x1 = 2 + (i * step);
            int y1 = 30 - (int)(((sm.pressureHistory[i] - minP) / (maxP - minP)) * 16.0f);
            int x2 = 2 + ((i + 1) * step);
            int y2 = 30 - (int)(((sm.pressureHistory[i + 1] - minP) / (maxP - minP)) * 16.0f);
            oled.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
        }
    }

    oled.setCursor(0, 34);
    oled.printf("TEMP: %.1fC (%.0f%% HUM)", sm.data.tempC, sm.data.humidity);
    oled.drawRoundRect(0, 44, 128, 20, 3, SSD1306_WHITE);
    oled.setCursor(4, 50);
    oled.print("INDOOR CLIMATE STABLE");
}

void OledDisplayManager::drawMode3_Marquee(const String& text) {
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);

    oled.fillRect(4, 3, 4, 6, SSD1306_WHITE);
    oled.fillTriangle(8, 3, 13, 0, 13, 11, SSD1306_WHITE);
    oled.drawPixel(15, 3, SSD1306_WHITE);
    oled.drawPixel(16, 5, SSD1306_WHITE);
    oled.drawPixel(15, 8, SSD1306_WHITE);

    oled.setCursor(22, 2);
    oled.print("ANNOUNCEMENT");
    oled.drawFastHLine(0, 13, 128, SSD1306_WHITE);

    oled.setTextWrap(false);
    oled.setTextSize(2);
    oled.setCursor(scrollX, 26);
    oled.print(text);
    oled.setTextWrap(true);

    scrollX -= 8;
    int textWidth = text.length() * 12;
    if (scrollX < -textWidth) {
        scrollX = 128;
    }

    oled.drawFastHLine(0, 50, 128, SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(10, 54);
    oled.print("CHAOSDESKY DESK HUB");
}

void OledDisplayManager::drawMode5_WifiInfo(const String& ipStr, const String& ssid, const String& pass) {
    oled.fillRoundRect(0, 0, 128, 14, 2, SSD1306_WHITE);
    oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(14, 3);
    oled.print("WIFI NETWORK INFO");
    
    oled.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    oled.setCursor(2, 18);
    oled.printf("IP  : %s", ipStr.c_str());
    oled.setCursor(2, 30);
    oled.printf("SSID: %s", ssid.c_str());
    oled.setCursor(2, 42);
    oled.printf("PASS: %s", pass.c_str());
    
    oled.drawRect(0, 53, 128, 11, SSD1306_WHITE);
    oled.setCursor(14, 55);
    oled.print("SCAN TFT FOR QR URL");
}

void OledDisplayManager::drawOledClock_Orbit(int h, int m, int s_val, const SensorData& s) {
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

void OledDisplayManager::drawOledClock_Word(int h, int m, int s_val, const SensorData& s) {
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

void OledDisplayManager::drawOledClock_DotMatrix(int h, int m, int s_val, const SensorData& s) {
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

void OledDisplayManager::drawOledClock_BarGraph(int h, int m, int s_val, const SensorData& s) {
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

void OledDisplayManager::drawOledClock_RadarSweep(int h, int m, int s_val, const SensorData& s) {
    int cx = 64, cy = 32, r = 30;
    oled.drawCircle(cx, cy, r, SSD1306_WHITE);
    oled.drawCircle(cx, cy, r - 10, SSD1306_WHITE);
    oled.drawCircle(cx, cy, r - 20, SSD1306_WHITE);

    oled.drawFastHLine(cx - r, cy, r * 2, SSD1306_WHITE);
    oled.drawFastVLine(cx, cy - r, r * 2, SSD1306_WHITE);

    float hAngle = (h % 12 + m / 60.0f) * 30.0f * (PI / 180.0f) - HALF_PI;
    float mAngle = m * 6.0f * (PI / 180.0f) - HALF_PI;
    float sAngle = s_val * 6.0f * (PI / 180.0f) - HALF_PI;

    oled.fillCircle(cx + cos(hAngle) * (r - 15), cy + sin(hAngle) * (r - 15), 3, SSD1306_WHITE);
    oled.fillCircle(cx + cos(mAngle) * (r - 5), cy + sin(mAngle) * (r - 5), 2, SSD1306_WHITE);

    oled.drawLine(cx, cy, cx + cos(sAngle) * r, cy + sin(sAngle) * r, SSD1306_WHITE);
}

void OledDisplayManager::drawOledClock_VintagePocket(int h, int m, int s_val, const SensorData& s) {
    int cx = 64, cy = 32, r = 30;
    oled.drawCircle(cx, cy, r, SSD1306_WHITE);
    oled.drawCircle(cx, cy, r - 2, SSD1306_WHITE);

    oled.setTextSize(1);
    oled.setCursor(cx - 5, cy - r + 4); oled.print("XII");
    oled.setCursor(cx - 3, cy + r - 10); oled.print("VI");
    oled.setCursor(cx + r - 16, cy - 3); oled.print("III");
    oled.setCursor(cx - r + 4, cy - 3); oled.print("IX");

    float hAngle = (h % 12 + m / 60.0f) * 30.0f * (PI / 180.0f) - HALF_PI;
    float mAngle = m * 6.0f * (PI / 180.0f) - HALF_PI;
    
    oled.drawLine(cx, cy, cx + cos(hAngle) * 14, cy + sin(hAngle) * 14, SSD1306_WHITE);
    oled.drawLine(cx, cy, cx + cos(mAngle) * 22, cy + sin(mAngle) * 22, SSD1306_WHITE);
    float sAngle = s_val * 6.0f * (PI / 180.0f) - HALF_PI;
    oled.drawPixel(cx + cos(sAngle) * 25, cy + sin(sAngle) * 25, SSD1306_WHITE);
}
