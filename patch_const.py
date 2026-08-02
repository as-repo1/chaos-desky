import re

# Fix pomodoro.h
with open("include/pomodoro.h", "r") as f:
    content = f.read()
content = content.replace("float getProgress() {", "float getProgress() const {")
content = content.replace("String getFormattedTime() {", "String getFormattedTime() const {")
content = content.replace("String getStateString() {", "String getStateString() const {")
with open("include/pomodoro.h", "w") as f:
    f.write(content)

# Fix display_tft.h
with open("include/display_tft.h", "r") as f:
    content = f.read()
content = content.replace("void renderPressureGraphPage(SensorManager& sm, bool fullRedraw)", "void renderPressureGraphPage(const SensorManager& sm, bool fullRedraw)")
content = content.replace("void renderEnvironmentalPage(SensorManager& sm, bool fullRedraw)", "void renderEnvironmentalPage(const SensorManager& sm, bool fullRedraw)")
with open("include/display_tft.h", "w") as f:
    f.write(content)

# Fix main.cpp
with open("src/main.cpp", "r") as f:
    content = f.read()
content = content.replace("tftMgr.renderCurrentPage(weatherMgr.weather, sensorMgr, pomoTimer, notificationMgr, localIpStr, timeStr, configMgr.config.oledMode, configMgr.config.oledClockStyle);", "tftMgr.renderCurrentPage(weatherMgr.weather, sensorMgr, pomoTimer, notificationMgr, h, m, s_val, localIpStr, configMgr.config.oledMode, configMgr.config.oledClockStyle);")
with open("src/main.cpp", "w") as f:
    f.write(content)

print("Const and signatures patched")
