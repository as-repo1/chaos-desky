import re

with open("include/display_tft.h", "r") as f:
    content = f.read()

find_str = """            // Bottom Right - Press
            tft.fillCircle(100, 120, 3, ST77XX_GREEN);
            tft.setCursor(62, 120); tft.printf("P:%.0f", s.pressureHpa);
        }
    }"""

replace_str = """            // Bottom Right - Press
            tft.fillCircle(100, 120, 3, ST77XX_GREEN);
            tft.setCursor(62, 120); tft.printf("P:%.0f", s.pressureHpa);

        } else if (activeClimateTheme == 4) {
            // Theme 4: Retro LCD
            if (fullRedraw) {
                tft.fillScreen(0xCE79); // Grayish-green LCD background
                tft.drawRect(2, 16, 124, 128, 0x4208); // Dark outline
                tft.drawRect(4, 18, 120, 124, 0x4208);
                
                tft.setTextColor(0x4208); // Dark LCD pixel color
                tft.setTextSize(1);
                tft.setCursor(8, 24); tft.print("TEMP");
                tft.setCursor(8, 76); tft.print("HUMIDITY");
                tft.drawLine(4, 70, 124, 70, 0x4208);
                
                tft.setCursor(8, 124); tft.print("PRS");
                tft.setCursor(64, 124); tft.print("ALT");
            }
            
            // Draw "88.8" faint background for realism
            tft.setTextColor(0xBEF7);
            tft.setTextSize(4);
            tft.setCursor(20, 38); tft.print("88.8");
            tft.setCursor(20, 90); tft.print("88.8");
            
            // Draw actual values
            tft.setTextColor(0x4208, 0xCE79);
            tft.setTextSize(4);
            tft.setCursor(20, 38); tft.printf("%.1f", s.tempC);
            tft.setCursor(20, 90); tft.printf("%.0f", s.humidity);
            
            tft.setTextSize(1);
            tft.setCursor(32, 124); tft.printf("%.0f", s.pressureHpa);
            tft.setCursor(88, 124); tft.printf("%.0f", s.altitudeM);

        } else if (activeClimateTheme == 5) {
            // Theme 5: Car Dashboard
            if (fullRedraw) {
                tft.fillScreen(ST77XX_BLACK);
                
                // Draw 2 large arcs for gauges
                for(int r = 24; r <= 26; r++) {
                    tft.drawCircle(32, 60, r, ST77XX_WHITE);
                    tft.drawCircle(96, 60, r, ST77XX_WHITE);
                }
                
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
                tft.setTextSize(1);
                tft.setCursor(22, 92); tft.print("TMP");
                tft.setCursor(86, 92); tft.print("HUM");
                
                tft.drawRect(4, 110, 120, 34, 0x18E3);
                tft.setCursor(8, 114); tft.print("DEW:");
                tft.setCursor(68, 114); tft.print("HTX:");
            }
            
            // Needle math
            float t_angle = map(s.tempC, 0, 50, 135, 405) * (M_PI / 180.0f);
            float h_angle = map(s.humidity, 0, 100, 135, 405) * (M_PI / 180.0f);
            
            // Draw needles (clear old by re-drawing black circle inside, then draw new)
            tft.fillCircle(32, 60, 22, ST77XX_BLACK);
            tft.fillCircle(96, 60, 22, ST77XX_BLACK);
            
            tft.drawLine(32, 60, 32 + cos(t_angle)*20, 60 + sin(t_angle)*20, ST77XX_RED);
            tft.drawLine(96, 60, 96 + cos(h_angle)*20, 60 + sin(h_angle)*20, ST77XX_CYAN);
            
            tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            tft.setTextSize(1);
            tft.setCursor(22, 68); tft.printf("%.1f", s.tempC);
            tft.setCursor(86, 68); tft.printf("%.0f", s.humidity);
            
            tft.setCursor(34, 114); tft.printf("%.1f", s.dewPointC);
            tft.setCursor(94, 114); tft.printf("%.1f", s.heatIndexC);

        } else if (activeClimateTheme == 6) {
            // Theme 6: Floating Bubbles (Modernized)
            if (fullRedraw) {
                tft.fillScreen(COLOR_BG);
                // Draw 4 distinct bubbles
                tft.fillRoundRect(8, 20, 52, 52, 26, ST77XX_MAGENTA);
                tft.fillRoundRect(68, 20, 52, 52, 26, ST77XX_CYAN);
                tft.fillRoundRect(8, 80, 52, 52, 26, ST77XX_ORANGE);
                tft.fillRoundRect(68, 80, 52, 52, 26, ST77XX_GREEN);
            }
            
            tft.setTextColor(ST77XX_WHITE);
            tft.setTextSize(1);
            
            // Bubble 1: Temp
            tft.setCursor(22, 34); tft.print("TMP");
            tft.setCursor(20, 46); tft.printf("%.1f", s.tempC);
            
            // Bubble 2: Hum
            tft.setCursor(82, 34); tft.print("HUM");
            tft.setCursor(82, 46); tft.printf("%.0f", s.humidity);
            
            // Bubble 3: Press
            tft.setCursor(22, 94); tft.print("PRS");
            tft.setCursor(18, 106); tft.printf("%.0f", s.pressureHpa);
            
            // Bubble 4: Alt
            tft.setCursor(82, 94); tft.print("ALT");
            tft.setCursor(80, 106); tft.printf("%.0f", s.altitudeM);
        }
    }"""

if find_str in content:
    content = content.replace(find_str, replace_str)
    with open("include/display_tft.h", "w") as f:
        f.write(content)
    print("Success")
else:
    print("Failed to find substring")
