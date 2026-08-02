import re

with open("include/watchface_engine.h", "r") as f:
    content = f.read()

# 1. Update Enum
content = content.replace("WATCHFACE_CASIO_F91W = 4,", "WATCHFACE_RETRO_FLIP = 4,")
content = content.replace("WATCHFACE_CASIO_GSHOCK = 5,", "WATCHFACE_BINARY_MATRIX = 5,")
content = content.replace("WATCHFACE_CASIO_CALCULATOR = 6,", "WATCHFACE_CRT_TERMINAL = 6,")
content = content.replace("WATCHFACE_CASIO_ROYALE = 7,", "WATCHFACE_ORBITAL_RINGS = 7,")
content = content.replace("WATCHFACE_CASIO_A168 = 10,", "WATCHFACE_STEAMPUNK_GAUGE = 10,")
content = content.replace("WATCHFACE_CASIO_DATABANK = 11,", "WATCHFACE_SEGMENT_LED = 11,")

content = content.replace("activeStyle = WATCHFACE_CASIO_F91W;", "activeStyle = WATCHFACE_RETRO_FLIP;")

# 2. Update switch statement cases
content = content.replace("case WATCHFACE_CASIO_F91W:\n                renderCasioF91W", "case WATCHFACE_RETRO_FLIP:\n                renderRetroFlip")
content = content.replace("case WATCHFACE_CASIO_GSHOCK:\n                renderCasioGShock", "case WATCHFACE_BINARY_MATRIX:\n                renderBinaryMatrix")
content = content.replace("case WATCHFACE_CASIO_CALCULATOR:\n                renderCasioCalculator", "case WATCHFACE_CRT_TERMINAL:\n                renderCRTTerminal")
content = content.replace("case WATCHFACE_CASIO_ROYALE:\n                renderCasioRoyale", "case WATCHFACE_ORBITAL_RINGS:\n                renderOrbitalRings")
content = content.replace("case WATCHFACE_CASIO_A168:\n                renderCasioA168", "case WATCHFACE_STEAMPUNK_GAUGE:\n                renderSteampunkGauge")
content = content.replace("case WATCHFACE_CASIO_DATABANK:\n                renderCasioDataBank", "case WATCHFACE_SEGMENT_LED:\n                renderSegmentLED")

# Default case fallback
content = content.replace("default:\n                renderCasioF91W", "default:\n                renderRetroFlip")

# 3. We need to replace the large Casio functions with new functions.
# To do this safely, we will use regex to find the functions and replace them.

def replace_func(content, old_func_name, new_func_name, new_body):
    pattern = r"void\s+" + old_func_name + r"\s*\(.*?}\s*(?=\n\s*(?:void\s+\w+|// =========================================================|private:|public:|// ---))"
    match = re.search(pattern, content, re.DOTALL)
    if match:
        content = content[:match.start()] + new_body + content[match.end():]
    else:
        print(f"Failed to find {old_func_name}")
    return content


func_retro_flip = """void renderRetroFlip(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, float tempC,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            // Draw flip cards
            gfx.fillRoundRect(10, 40, 48, 60, 4, 0x3186); // Dark grey
            gfx.fillRoundRect(70, 40, 48, 60, 4, 0x3186);
            // Center split line
            gfx.drawLine(8, 70, 120, 70, colorBg);
        }
        
        gfx.setTextColor(0xFFFF, 0x3186); // White text on dark grey
        gfx.setTextSize(4);
        gfx.setCursor(14, 55);
        gfx.printf("%02d", h);
        gfx.setCursor(74, 55);
        gfx.printf("%02d", m);
        
        // Seconds ticking below
        gfx.setTextColor(colorAccent, colorBg);
        gfx.setTextSize(1);
        gfx.setCursor(55, 110);
        gfx.printf(":%02d", s);
    }
"""

func_binary_matrix = """void renderBinaryMatrix(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, float tempC,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, 0x0000); // Black background
            gfx.setTextColor(0x07E0); // Bright green
            gfx.setTextSize(1);
            gfx.setCursor(10, 20);
            gfx.print("H M S");
            gfx.drawLine(10, 30, 118, 30, 0x07E0);
        }
        
        uint16_t c_on = 0x07E0;
        uint16_t c_off = 0x0180; // Dark green
        
        int vals[3] = {h, m, s};
        int x_offsets[3] = {16, 56, 96};
        
        for (int c = 0; c < 3; c++) {
            for (int r = 0; r < 6; r++) {
                int bit = (vals[c] >> (5 - r)) & 1;
                uint16_t color = bit ? c_on : c_off;
                gfx.fillCircle(x_offsets[c], 45 + r * 15, 5, color);
            }
        }
    }
"""

func_crt_terminal = """void renderCRTTerminal(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, float tempC,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, 0x0000);
            for (int i=16; i<160; i+=4) {
                gfx.drawFastHLine(0, i, 128, 0x0180); // scanlines
            }
        }
        gfx.setTextColor(0x07E0, 0x0000);
        gfx.setTextSize(1);
        gfx.setCursor(10, 40);
        gfx.print("user@desky:~$ time");
        
        gfx.setTextSize(2);
        gfx.setCursor(10, 60);
        gfx.printf("%02d:%02d:%02d", h, m, s);
        
        gfx.setTextSize(1);
        gfx.setCursor(10, 90);
        if (s % 2 == 0) gfx.print("_"); else gfx.print(" ");
    }
"""

func_orbital_rings = """void renderOrbitalRings(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, float tempC,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        int cx = 64, cy = 88;
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, colorBg);
            gfx.drawCircle(cx, cy, 40, 0x2104);
            gfx.drawCircle(cx, cy, 30, 0x2104);
            gfx.drawCircle(cx, cy, 20, 0x2104);
        }
        
        // Simple orbital pointers
        // Erase old
        if (lastS >= 0) {
            float os = lastS * M_PI / 30.0 - M_PI / 2.0;
            gfx.fillCircle(cx + cos(os)*40, cy + sin(os)*40, 3, 0x2104);
            float om = lastM * M_PI / 30.0 - M_PI / 2.0;
            gfx.fillCircle(cx + cos(om)*30, cy + sin(om)*30, 4, 0x2104);
            float oh = (lastH%12) * M_PI / 6.0 - M_PI / 2.0;
            gfx.fillCircle(cx + cos(oh)*20, cy + sin(oh)*20, 5, 0x2104);
        }
        
        float ah = (h%12) * M_PI / 6.0 - M_PI / 2.0;
        gfx.fillCircle(cx + cos(ah)*20, cy + sin(ah)*20, 5, colorAccent);
        
        float am = m * M_PI / 30.0 - M_PI / 2.0;
        gfx.fillCircle(cx + cos(am)*30, cy + sin(am)*30, 4, colorPrimary);
        
        float a_s = s * M_PI / 30.0 - M_PI / 2.0;
        gfx.fillCircle(cx + cos(a_s)*40, cy + sin(a_s)*40, 3, colorText);
        
        gfx.setTextSize(1);
        gfx.setTextColor(colorText, colorBg);
        gfx.setCursor(cx-18, cy-4);
        gfx.printf("%02d:%02d", h, m);
    }
"""

func_steampunk_gauge = """void renderSteampunkGauge(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, float tempC,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        int cx = 64, cy = 88;
        uint16_t copper = 0xFD20; // Orange/Brownish
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, 0x18C3); // Dark slate
            gfx.fillCircle(cx, cy, 45, 0x0000);
            gfx.drawCircle(cx, cy, 45, copper);
            gfx.drawCircle(cx, cy, 44, copper);
            for(int i=0; i<360; i+=15) {
                float rad = i * M_PI / 180.0;
                gfx.drawLine(cx + cos(rad)*40, cy + sin(rad)*40, cx + cos(rad)*44, cy + sin(rad)*44, copper);
            }
        }
        
        // Erase
        if (lastS >= 0) {
            float os = lastS * M_PI / 30.0 - M_PI / 2.0;
            gfx.drawLine(cx, cy, cx + cos(os)*38, cy + sin(os)*38, 0x0000);
            float om = lastM * M_PI / 30.0 - M_PI / 2.0;
            gfx.drawLine(cx, cy, cx + cos(om)*30, cy + sin(om)*30, 0x0000);
            float oh = (lastH%12) * M_PI / 6.0 - M_PI / 2.0;
            gfx.drawLine(cx, cy, cx + cos(oh)*20, cy + sin(oh)*20, 0x0000);
        }
        
        float ah = (h%12) * M_PI / 6.0 - M_PI / 2.0;
        gfx.drawLine(cx, cy, cx + cos(ah)*20, cy + sin(ah)*20, 0xFFFF);
        float am = m * M_PI / 30.0 - M_PI / 2.0;
        gfx.drawLine(cx, cy, cx + cos(am)*30, cy + sin(am)*30, 0xCE79);
        float a_s = s * M_PI / 30.0 - M_PI / 2.0;
        gfx.drawLine(cx, cy, cx + cos(a_s)*38, cy + sin(a_s)*38, copper);
        gfx.fillCircle(cx, cy, 4, copper);
    }
"""

func_segment_led = """void renderSegmentLED(Adafruit_GFX& gfx, int h, int m, int s,
                             const OutdoorWeatherData& w, float tempC,
                             uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw) {
        if (fullRedraw) {
            gfx.fillRect(0, 16, 128, 144, 0x0000);
        }
        
        gfx.setTextColor(0xF800, 0x0000); // Pure Red
        gfx.setTextSize(4);
        gfx.setCursor(8, 50);
        gfx.printf("%02d", h);
        
        if (s % 2 == 0) gfx.print(":"); else gfx.print(" ");
        gfx.setCursor(68, 50);
        gfx.printf("%02d", m);
        
        gfx.setTextSize(2);
        gfx.setCursor(55, 100);
        gfx.printf("%02d", s);
    }
"""

content = replace_func(content, "renderCasioF91W", "renderRetroFlip", func_retro_flip)
content = replace_func(content, "renderCasioGShock", "renderBinaryMatrix", func_binary_matrix)
content = replace_func(content, "renderCasioCalculator", "renderCRTTerminal", func_crt_terminal)
content = replace_func(content, "renderCasioRoyale", "renderOrbitalRings", func_orbital_rings)
content = replace_func(content, "renderCasioA168", "renderSteampunkGauge", func_steampunk_gauge)
content = replace_func(content, "renderCasioDataBank", "renderSegmentLED", func_segment_led)

with open("include/watchface_engine.h", "w") as f:
    f.write(content)

