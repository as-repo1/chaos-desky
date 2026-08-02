import re

with open("include/display_tft.h", "r") as f:
    content = f.read()

# 1. Update renderCurrentPage switch block
switch_old = """        switch (currentPage) {
            case 0: renderOutdoorWeatherPage(weather, fullRedraw); break;
            case 1: renderPressureGraphPage(sensors, fullRedraw); break;
            case 2: renderPomodoroPage(pomo, h, m, fullRedraw); break;
            case 3: renderSystemQrPage(ipStr, fullRedraw); break;
            case 4: renderCustomUserPage(fullRedraw); break;
            case 5: renderIndoorClimatePage(sensors.data, fullRedraw); break;
            case 6: renderBigClockPage(h, m, s, weather, sensors, pomo, fullRedraw); break;
            case 7: renderNetworkMonitorPage(ipStr, fullRedraw); break;
            case 8: renderSystemHardwarePage(fullRedraw); break;
            case 9: renderOledControlPage(fullRedraw, oledMode, oledClockStyle); break;
        }"""
switch_new = """        switch (currentPage) {
            case 0: renderOutdoorWeatherPage(weather, fullRedraw); break;
            case 1: renderHistoryGraphPage(sensors, 0, fullRedraw); break; // Pressure
            case 2: renderPomodoroPage(pomo, h, m, fullRedraw); break;
            case 3: renderSystemQrPage(ipStr, fullRedraw); break;
            case 4: renderCustomUserPage(fullRedraw); break;
            case 5: renderIndoorClimatePage(sensors.data, fullRedraw); break;
            case 6: renderBigClockPage(h, m, s, weather, sensors, pomo, fullRedraw); break;
            case 7: renderNetworkMonitorPage(ipStr, fullRedraw); break;
            case 8: renderSystemHardwarePage(fullRedraw); break;
            case 9: renderOledControlPage(fullRedraw, oledMode, oledClockStyle); break;
            case 10: renderHistoryGraphPage(sensors, 1, fullRedraw); break; // Temp
            case 11: renderHistoryGraphPage(sensors, 2, fullRedraw); break; // Hum
            case 12: renderComfortIndexPage(sensors.data, fullRedraw); break; // Comfort Index
        }"""
content = content.replace(switch_old, switch_new)

# 2. Rename renderPressureGraphPage to renderHistoryGraphPage and make it accept graphType
graph_func_old = "void renderPressureGraphPage(const SensorManager& sm, bool fullRedraw) {"
graph_func_new = "void renderHistoryGraphPage(const SensorManager& sm, int graphType, bool fullRedraw) {"
content = content.replace(graph_func_old, graph_func_new)

# Replace all uses of activeGraphType in that function with graphType
content = re.sub(r'activeGraphType', 'graphType', content)

# 3. Add renderComfortIndexPage
comfort_func = """
    // --- Page 12: Comfort Index & Air Quality ---
    void renderComfortIndexPage(const SensorData& sd, bool fullRedraw) {
        if (fullRedraw) {
            tft.fillRoundRect(4, 16, 120, 20, 4, COLOR_PRIMARY);
            tft.setTextColor(COLOR_BG, COLOR_PRIMARY);
            tft.setTextSize(1);
            tft.setCursor(8, 22);
            tft.print("Comfort Index");

            // Heat Index Section
            tft.drawRoundRect(4, 40, 120, 48, 6, COLOR_PRIMARY);
            tft.setTextColor(COLOR_TEXT, COLOR_BG);
            tft.setCursor(8, 46);
            tft.print("Feels Like:");
            tft.setTextSize(2);
            tft.setCursor(8, 62);
            
            uint16_t heatColor = COLOR_GOOD;
            if (sd.heatIndexC > 30.0f) heatColor = COLOR_ALERT;
            else if (sd.heatIndexC > 26.0f) heatColor = COLOR_WARN;
            else if (sd.heatIndexC < 15.0f) heatColor = 0x07FF; // Cyan
            
            tft.setTextColor(heatColor, COLOR_BG);
            tft.printf("%.1fC", sd.heatIndexC);

            // Dew Point Section
            tft.drawRoundRect(4, 92, 120, 48, 6, COLOR_PRIMARY);
            tft.setTextSize(1);
            tft.setTextColor(COLOR_TEXT, COLOR_BG);
            tft.setCursor(8, 98);
            tft.print("Dew Point:");
            tft.setTextSize(2);
            tft.setCursor(8, 114);
            
            uint16_t dewColor = COLOR_GOOD;
            if (sd.dewPointC > 20.0f) dewColor = COLOR_ALERT; // Muggy
            else if (sd.dewPointC < 10.0f) dewColor = 0x07FF; // Dry
            
            tft.setTextColor(dewColor, COLOR_BG);
            tft.printf("%.1fC", sd.dewPointC);
            
            // Emoji indicators
            tft.setTextSize(1);
            if (sd.heatIndexC > 30.0f) {
                GfxIconRenderer::drawSun(tft, 100, 60, COLOR_ALERT);
            } else if (sd.heatIndexC < 15.0f) {
                // Draw snowflake-ish
                tft.drawLine(100, 56, 100, 64, 0x07FF);
                tft.drawLine(96, 60, 104, 60, 0x07FF);
                tft.drawLine(97, 57, 103, 63, 0x07FF);
                tft.drawLine(97, 63, 103, 57, 0x07FF);
            } else {
                // Smiley
                tft.drawCircle(100, 60, 6, COLOR_GOOD);
                tft.drawPixel(98, 58, COLOR_GOOD);
                tft.drawPixel(102, 58, COLOR_GOOD);
                tft.drawLine(98, 62, 102, 62, COLOR_GOOD);
            }
            
            if (sd.dewPointC > 20.0f) {
                // Water drop
                tft.fillTriangle(100, 108, 96, 116, 104, 116, 0x07FF);
                tft.fillCircle(100, 116, 4, 0x07FF);
            } else if (sd.dewPointC < 10.0f) {
                tft.setCursor(96, 112);
                tft.setTextColor(COLOR_TEXT);
                tft.print("DRY");
            } else {
                tft.setCursor(96, 112);
                tft.setTextColor(COLOR_GOOD);
                tft.print("OK");
            }
        }
    }
"""

content = content + comfort_func

with open("include/display_tft.h", "w") as f:
    f.write(content)

print("Display TFT patched")
