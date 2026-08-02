import re

with open("include/watchface_engine.h", "r") as f:
    content = f.read()

# Pattern for renderSwissAnalog
p_swiss = re.compile(r'(void renderSwissAnalog\(.*?bool fullRedraw\) \{).*?(    // --- Style 1: Cyberpunk Dual Chronograph ---)', re.DOTALL)
new_swiss = r"""\1
        int cx = 64, cy = 72, r = 44;

        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            // Sleek Outer Bezel
            gfx.drawCircle(cx, cy, r + 2, colorPrimary);
            
            // Minimalist ticks
            for (int i = 0; i < 60; i++) {
                float angle = i * (2.0f * M_PI / 60.0f);
                if (i % 5 == 0) {
                    gfx.drawLine(cx + (int)(cos(angle) * (r - 6)), cy + (int)(sin(angle) * (r - 6)),
                                 cx + (int)(cos(angle) * (r - 2)), cy + (int)(sin(angle) * (r - 2)), colorAccent);
                } else {
                    gfx.drawPixel(cx + (int)(cos(angle) * (r - 3)), cy + (int)(sin(angle) * (r - 3)), colorPrimary);
                }
            }

            // Trend & Pomo Indicators
            gfx.setTextSize(1);
            gfx.setTextColor(colorAccent, colorBg);
            gfx.setCursor(14, 124);
            if (pomo.isActive) gfx.printf("POMO: %s", pomo.isWorkPhase ? "FOCUS" : "BREAK");
            else gfx.print("POMO: IDLE");

            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(14, 138);
            gfx.printf("PRS: %d hPa", (int)sensors.data.pressureHPa);
        }

        // Fast Clear old hands (Inner circle)
        gfx.fillCircle(cx, cy, r - 7, colorBg);
        
        // Pomo Ring Micro-animation (if active, draw a small arc)
        if (pomo.isActive && pomo.totalPhaseTime > 0) {
            float progress = 1.0f - ((float)pomo.timeLeft / pomo.totalPhaseTime);
            int pAngle = (int)(progress * 360);
            for(int a=0; a<pAngle; a+=5) {
                float rad = (a - 90) * M_PI / 180.0f;
                gfx.drawPixel(cx + (int)(cos(rad)*15), cy + (int)(sin(rad)*15), colorAccent);
            }
        }

        // Draw Hands
        float angleH = ((h % 12) + m / 60.0f) * (2.0f * M_PI / 12.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleH) * 20), cy + (int)(sin(angleH) * 20), colorPrimary);
        
        float angleM = (m + s / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleM) * 32), cy + (int)(sin(angleM) * 32), colorText);
        
        float angleS = s * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleS) * 38), cy + (int)(sin(angleS) * 38), colorAccent);
        
        gfx.fillCircle(cx, cy, 2, colorPrimary);
    }

\2"""
content = p_swiss.sub(new_swiss, content)

# Pattern for renderCyberChrono
p_cyber = re.compile(r'(void renderCyberChrono\(.*?bool fullRedraw\) \{).*?(    // --- Style 2: Modern Minimalist Digital \(Fixed UI & Zero Overlap\) ---)', re.DOTALL)
new_cyber = r"""\1
        int cx = 64, cy = 84, r = 32;

        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            gfx.drawCircle(cx, cy, r, colorPrimary);
            gfx.drawRoundRect(4, 126, 120, 24, 4, colorPrimary);
            gfx.setTextSize(1);
            gfx.setTextColor(colorText, colorBg);
            gfx.setCursor(10, 130);
            gfx.printf("IN: %.1f OUT: %.1f", sensors.data.tempC, w.temp);
            gfx.setCursor(10, 140);
            gfx.printf("POMO: %s", pomo.isActive ? (pomo.isWorkPhase ? "FOCUS" : "BREAK") : "IDLE");
        }

        // Digital Time Header with Blinking Colon
        gfx.setTextSize(2);
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setCursor(16, 24);
        if (s % 2 == 0) gfx.printf("%02d:%02d:%02d", h, m, s);
        else gfx.printf("%02d %02d %02d", h, m, s);

        // Erase sub-dial interior
        gfx.fillCircle(cx, cy, r - 2, colorBg);

        // Active Pomodoro Circular Ring
        if (pomo.isActive && pomo.totalPhaseTime > 0) {
            float progress = 1.0f - ((float)pomo.timeLeft / pomo.totalPhaseTime);
            int pAngle = (int)(progress * 360);
            for(int a=0; a<pAngle; a+=2) {
                float rad = (a - 90) * M_PI / 180.0f;
                gfx.drawPixel(cx + (int)(cos(rad)*28), cy + (int)(sin(rad)*28), colorPrimary);
            }
        }

        // Analog Hands
        float angleM = (m + s / 60.0f) * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleM) * 22), cy + (int)(sin(angleM) * 22), colorText);

        float angleS = s * (2.0f * M_PI / 60.0f) - (M_PI / 2.0f);
        gfx.drawLine(cx, cy, cx + (int)(cos(angleS) * 28), cy + (int)(sin(angleS) * 28), colorAccent);
        gfx.fillCircle(cx, cy, 2, colorAccent);
    }

\2"""
content = p_cyber.sub(new_cyber, content)


# Pattern for renderModernDigital
p_modern = re.compile(r'(void renderModernDigital\(.*?bool fullRedraw\) \{).*?(    // --- Style 3: Neon Nixie Tube Simulation ---)', re.DOTALL)
new_modern = r"""\1
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            
            // Clean top title badge
            gfx.fillRoundRect(6, 20, 116, 18, 4, colorPrimary);
            gfx.setTextColor(colorBg, colorPrimary);
            gfx.setTextSize(1);
            gfx.setCursor(10, 25);
            gfx.print("MODERN DIGITAL HUD");

            // Main digital time container frame
            gfx.drawRoundRect(4, 42, 120, 48, 6, colorAccent);
            gfx.drawRoundRect(5, 43, 118, 46, 5, colorPrimary);
            
            // Environmental Telemetry Box below
            gfx.drawRoundRect(4, 94, 120, 60, 6, colorPrimary);
        }

        // Overdraw dynamic telemetry values to prevent flicker
        gfx.fillRect(6, 96, 116, 56, colorBg);
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(1);
        gfx.setCursor(10, 102);
        gfx.printf("ENV: %s", w.condition.substring(0, 12).c_str());
        gfx.setCursor(10, 116);
        gfx.printf("IN %.1fC OUT %.1fC", sensors.data.tempC, w.temp);
        
        // Pomo & Trends
        gfx.setCursor(10, 130);
        gfx.setTextColor(colorAccent, colorBg);
        if (pomo.isActive) gfx.printf("POMO %s %02d:%02d", pomo.isWorkPhase ? "FOCUS" : "BREAK", pomo.timeLeft / 60, pomo.timeLeft % 60);
        else gfx.printf("POMO IDLE  %dhPa", (int)sensors.data.pressureHPa);

        // Clean time rendering with exact zero-overlap margins
        gfx.setTextColor(colorText, colorBg);
        gfx.setTextSize(3);
        gfx.setCursor(8, 54);
        gfx.printf("%02d:%02d", h, m);

        // Separated sleek second pillbox
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(2);
        gfx.fillRect(98, 60, 24, 16, colorBg);
        gfx.setCursor(98, 60);
        gfx.printf("%02d", s);
    }

\2"""
content = p_modern.sub(new_modern, content)

with open("include/watchface_engine.h", "w") as f:
    f.write(content)

print("Modernization patch applied.")
