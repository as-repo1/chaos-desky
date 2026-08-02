import re

with open("include/display_tft.h", "r") as f:
    content = f.read()

# Find the start of the function
start_marker = "    // --- Page 5: Detailed Indoor Climate Analysis ---\n    void renderIndoorClimatePage(const SensorData& s, bool fullRedraw) {"
end_marker = "    // --- Page 6: Custom Watch Face Dial ---\n"

start_idx = content.find(start_marker)
end_idx = content.find(end_marker)

if start_idx == -1 or end_idx == -1:
    print("Could not find markers!")
    exit(1)

new_func = """    // --- Page 5: Detailed Indoor Climate Analysis ---
    void renderIndoorClimatePage(const SensorData& s, bool fullRedraw) {
        if (activeClimateTheme == 0) {
            // Theme 0: Neon Rings (Circular UI)
            if (fullRedraw) {
                tft.fillScreen(COLOR_BG);
                // Outer arcs (static background arcs)
                for(int r = 38; r <= 42; r++) {
                    tft.drawCircle(64, 46, r, 0x18E3); // Dark Gray
                }
                for(int r = 26; r <= 30; r++) {
                    tft.drawCircle(64, 46, r, 0x18E3); // Dark Gray
                }
                
                tft.setTextColor(COLOR_TEXT, COLOR_BG);
                tft.setTextSize(1);
                tft.setCursor(6, 102); tft.print("PRS:");
                tft.setCursor(6, 114); tft.print("DEW:");
                tft.setCursor(68, 102); tft.print("ALT:");
                tft.setCursor(68, 114); tft.print("H.I:");
            }
            
            // Draw active arcs (fake it with a thick filled arc or just colored circles for now)
            uint16_t tempColor = (s.tempC > 30) ? ST7735_RED : (s.tempC < 15 ? ST7735_CYAN : ST7735_ORANGE);
            uint16_t humColor = ST7735_CYAN;
            
            // Outer Temp Ring (dynamic) - simplified as drawing a coloured circle over the gray one if threshold met
            // We'll just draw some tick marks around the circle to represent value
            int tempTicks = map(s.tempC, 0, 50, 0, 36);
            for(int i = 0; i < 36; i++) {
                float angle = i * 10 * (M_PI / 180.0f) - M_PI / 2.0f;
                uint16_t c = (i < tempTicks) ? tempColor : 0x18E3;
                tft.drawLine(64 + cos(angle)*38, 46 + sin(angle)*38, 64 + cos(angle)*42, 46 + sin(angle)*42, c);
            }
            
            int humTicks = map(s.humidity, 0, 100, 0, 36);
            for(int i = 0; i < 36; i++) {
                float angle = i * 10 * (M_PI / 180.0f) - M_PI / 2.0f;
                uint16_t c = (i < humTicks) ? humColor : 0x18E3;
                tft.drawLine(64 + cos(angle)*26, 46 + sin(angle)*26, 64 + cos(angle)*30, 46 + sin(angle)*30, c);
            }

            tft.setTextColor(COLOR_TEXT, COLOR_BG);
            tft.setTextSize(2);
            tft.setCursor(40, 38);
            tft.printf("%.0f", s.tempC);
            
            tft.setTextSize(1);
            tft.setTextColor(COLOR_PRIMARY, COLOR_BG);
            tft.setCursor(30, 102); tft.printf("%4.0f", s.pressureHpa);
            tft.setCursor(30, 114); tft.printf("%4.1f", s.dewPointC);
            tft.setCursor(92, 102); tft.printf("%4.0f", s.altitudeM);
            tft.setCursor(92, 114); tft.printf("%4.1f", s.heatIndexC);

        } else if (activeClimateTheme == 1) {
            // Theme 1: Pip-Boy (Retro Terminal)
            uint16_t pipGreen = 0x07E0; // Bright Green
            if (fullRedraw) {
                tft.fillScreen(ST77XX_BLACK);
                tft.drawRect(2, 16, 124, 128, pipGreen);
                tft.drawRect(4, 18, 120, 124, pipGreen);
                
                tft.setTextColor(pipGreen, ST77XX_BLACK);
                tft.setTextSize(1);
                tft.setCursor(8, 22); tft.print("ROBCO INDUSTRIES");
                tft.drawLine(8, 32, 120, 32, pipGreen);
                
                tft.setCursor(8, 40); tft.print("> SENSORS ACTIVE");
            }
            
            tft.setTextColor(pipGreen, ST77XX_BLACK);
            tft.setTextSize(1);
            tft.setCursor(8, 56); tft.printf("[TMP] %5.1f C", s.tempC);
            tft.setCursor(8, 70); tft.printf("[HUM] %5.1f %%", s.humidity);
            tft.setCursor(8, 84); tft.printf("[PRS] %5.0f HPA", s.pressureHpa);
            tft.setCursor(8, 98); tft.printf("[ALT] %5.0f M", s.altitudeM);
            tft.setCursor(8, 112); tft.printf("[DEW] %5.1f C", s.dewPointC);
            tft.setCursor(8, 126); tft.printf("[HTX] %5.1f C", s.heatIndexC);
            
            // Blinking cursor
            if ((millis() / 500) % 2 == 0) {
                tft.fillRect(94, 126, 6, 8, pipGreen);
            } else {
                tft.fillRect(94, 126, 6, 8, ST77XX_BLACK);
            }

        } else if (activeClimateTheme == 2) {
            // Theme 2: Material Cards
            if (fullRedraw) {
                tft.fillScreen(0xEF7D); // Light Gray Background
                
                // Temp Card
                tft.fillRoundRect(4, 18, 120, 46, 6, ST77XX_WHITE);
                // Humidity Card
                tft.fillRoundRect(4, 68, 120, 34, 6, ST77XX_WHITE);
                // Grid Cards
                tft.fillRoundRect(4, 106, 58, 36, 6, ST77XX_WHITE);
                tft.fillRoundRect(66, 106, 58, 36, 6, ST77XX_WHITE);
                
                tft.setTextColor(0x8410); // Dark Gray Text
                tft.setTextSize(1);
                tft.setCursor(10, 22); tft.print("Temperature");
                tft.setCursor(10, 72); tft.print("Humidity");
                
                tft.setCursor(10, 110); tft.print("Pressure");
                tft.setCursor(72, 110); tft.print("Heat Idx");
            }
            
            tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
            tft.setTextSize(3);
            tft.setCursor(24, 34); tft.printf("%.1f", s.tempC);
            
            // Hum bar
            tft.drawRect(10, 86, 108, 10, 0xCE59); // Gray outline
            tft.fillRect(11, 87, 106, 8, ST77XX_WHITE); // Clear
            int humW = (s.humidity / 100.0f) * 106;
            tft.fillRect(11, 87, humW, 8, 0x03E0); // Blue fill
            tft.setTextSize(1);
            tft.setCursor(96, 72); tft.printf("%.0f%%", s.humidity);
            
            tft.setTextSize(1);
            tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
            tft.setCursor(10, 124); tft.printf("%.0f", s.pressureHpa);
            tft.setCursor(72, 124); tft.printf("%.1f", s.heatIndexC);

        } else if (activeClimateTheme == 3) {
            // Theme 3: Tactical Radar
            if (fullRedraw) {
                tft.fillScreen(0x0028); // Very dark blue/green
                tft.drawRect(2, 16, 124, 128, 0x07FF);
                
                // Draw radar grid
                for(int i=16; i<=144; i+=16) tft.drawLine(2, i, 126, i, 0x01AA);
                for(int i=2; i<=126; i+=16) tft.drawLine(i, 16, i, 144, 0x01AA);
                
                // Center crosshairs
                tft.drawLine(64, 16, 64, 144, 0x07FF);
                tft.drawLine(2, 80, 126, 80, 0x07FF);
                tft.drawCircle(64, 80, 40, 0x03E0);
            }
            
            // Simulated sweeping radar line
            static float angle = 0;
            angle += 0.2f;
            if(angle > M_PI*2) angle = 0;
            
            // Erase old line (approximate)
            float oldAngle = angle - 0.2f;
            tft.drawLine(64, 80, 64 + cos(oldAngle)*60, 80 + sin(oldAngle)*60, 0x0028);
            // Redraw grid over old line
            tft.drawCircle(64, 80, 40, 0x03E0);
            tft.drawLine(64, 16, 64, 144, 0x07FF);
            tft.drawLine(2, 80, 126, 80, 0x07FF);
            
            // Draw new line
            tft.drawLine(64, 80, 64 + cos(angle)*60, 80 + sin(angle)*60, 0x07FF);

            // Floating data points
            tft.setTextColor(ST77XX_WHITE, 0x0028);
            tft.setTextSize(1);
            
            // Top Left - Temp
            tft.fillCircle(24, 40, 3, ST77XX_RED);
            tft.setCursor(30, 36); tft.printf("T:%.1f", s.tempC);
            
            // Top Right - Hum
            tft.fillCircle(104, 40, 3, ST77XX_CYAN);
            tft.setCursor(68, 36); tft.printf("H:%.0f%%", s.humidity);
            
            // Bottom Left - Dew
            tft.fillCircle(30, 116, 3, ST77XX_YELLOW);
            tft.setCursor(36, 112); tft.printf("D:%.1f", s.dewPointC);
            
            // Bottom Right - Press
            tft.fillCircle(100, 120, 3, ST77XX_GREEN);
            tft.setCursor(62, 120); tft.printf("P:%.0f", s.pressureHpa);
        }
    }

"""

content = content[:start_idx] + new_func + content[end_idx:]

with open("include/display_tft.h", "w") as f:
    f.write(content)
print("Updated display_tft.h")
