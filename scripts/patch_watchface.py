import re

with open("include/watchface_engine.h", "r") as f:
    content = f.read()

# Add includes
if '#include "sensors.h"' not in content:
    content = content.replace('#include "weather_api.h"', '#include "weather_api.h"\n#include "sensors.h"\n#include "pomodoro.h"')

# Update enum
content = content.replace("WATCHFACE_SEIKO_DIVER = 8", "WATCHFACE_AVIATION_ALTIMETER = 8")
content = content.replace("WATCHFACE_PULSAR_LED = 9", "WATCHFACE_MINIMALIST_EINK = 9")

# Update render call in switch case
content = content.replace("case WATCHFACE_SEIKO_DIVER:\n                renderSeikoDiver", "case WATCHFACE_AVIATION_ALTIMETER:\n                renderAviationAltimeter")
content = content.replace("case WATCHFACE_PULSAR_LED:\n                renderPulsarLED", "case WATCHFACE_MINIMALIST_EINK:\n                renderMinimalistEink")

# Function renaming
content = content.replace("renderSeikoDiver", "renderAviationAltimeter")
content = content.replace("renderPulsarLED", "renderMinimalistEink")

# Update main render signature
old_sig = """void render(Adafruit_GFX& gfx, 
                int hours, int mins, int secs, 
                const OutdoorWeatherData& weather,
                float tempC,
                uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg,
                bool fullRedraw = false)"""
new_sig = """void render(Adafruit_GFX& gfx, 
                int hours, int mins, int secs, 
                const OutdoorWeatherData& weather,
                SensorManager& sensors, PomodoroTimer& pomo,
                uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg,
                bool fullRedraw = false)"""
content = content.replace(old_sig, new_sig)

# Update internal render calls in main render function
content = re.sub(r'render([A-Za-z0-9_]+)\(gfx, hours, mins, secs, weather, tempC,', r'render\1(gfx, hours, mins, secs, weather, sensors, pomo,', content)

# Update internal function signatures
# They look like: void renderXYZ(Adafruit_GFX& gfx, int h, int m, int s, const OutdoorWeatherData& weather, float tempC, uint16_t colorPrimary, uint16_t colorAccent, uint16_t colorText, uint16_t colorBg, bool fullRedraw)
content = re.sub(
    r'void render([A-Za-z0-9_]+)\(Adafruit_GFX& gfx,\s*int h,\s*int m,\s*int s,\s*const OutdoorWeatherData& weather,\s*float tempC,',
    r'void render\1(Adafruit_GFX& gfx, int h, int m, int s, const OutdoorWeatherData& weather, SensorManager& sensors, PomodoroTimer& pomo,',
    content
)

with open("include/watchface_engine.h", "w") as f:
    f.write(content)

print("Patch applied successfully.")
