import re

with open("include/display_oled.h", "r") as f:
    content = f.read()

# Replace timeStr with h, m, s_val in drawMode1_BigClock
content = content.replace(
    "void drawMode1_BigClock(int h, int m, int s_val, const String& ipStr, const SensorData& s, int style) {",
    "void drawMode1_BigClock(int h, int m, int s_val, const String& ipStr, const SensorData& s, int style) {"
)

# In drawMode1_BigClock, change the case calls
content = re.sub(r'drawOledClock_([A-Za-z0-9_]+)\(timeStr', r'drawOledClock_\1(h, m, s_val', content)

# Change signatures of drawOledClock_*
content = re.sub(r'void drawOledClock_([A-Za-z0-9_]+)\(const String& timeStr', r'void drawOledClock_\1(int h, int m, int s_val', content)

with open("include/display_oled.h", "w") as f:
    f.write(content)
print("OLED patched")
