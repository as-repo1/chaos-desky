import re

######################
# display_oled.h
######################
with open("include/display_oled.h", "r") as f:
    oled_content = f.read()

oled_content = oled_content.replace(
    "void draw(SensorManager& sm, const NotificationManager& notifMgr, const String& ipStr, const String& timeStr, int rssi, int clockStyle = 0)",
    "void draw(SensorManager& sm, const NotificationManager& notifMgr, const String& ipStr, int h, int m, int s_val, int rssi, int clockStyle = 0)"
)
# Update lastTime check
oled_content = oled_content.replace(
    "timeStr != lastTimeStr",
    "(h != lastH || m != lastM || s_val != lastS)"
)
oled_content = oled_content.replace(
    "lastTimeStr = timeStr;",
    "lastH = h; lastM = m; lastS = s_val;"
)
oled_content = oled_content.replace(
    "String lastTimeStr = \"\";",
    "int lastH = -1, lastM = -1, lastS = -1;"
)
oled_content = oled_content.replace("const String& timeStr", "int h, int m, int s_val")

# drawMode0_HUD
oled_content = oled_content.replace(
    "oled.print(timeStr);",
    "oled.printf(\"%02d:%02d:%02d\", h, m, s_val);"
)

# Style 0
oled_content = oled_content.replace(
    "oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : \"00:00\");",
    "oled.printf(\"%02d:%02d\", h, m);"
)
oled_content = oled_content.replace(
    "int sec = timeStr.length() >= 8 ? timeStr.substring(6, 8).toInt() : 0;",
    "int sec = s_val;"
)

# Style 1
oled_content = oled_content.replace(
    "oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : \"00:00\");",
    "oled.printf(\"%02d:%02d\", h, m);"
)
oled_content = oled_content.replace(
    "oled.print(timeStr.length() >= 8 ? timeStr.substring(6, 8) : \"00\");",
    "oled.printf(\"%02d\", s_val);"
)

# Style 2
oled_content = oled_content.replace(
    "oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : \"00:00\");",
    "oled.printf(\"%02d:%02d\", h, m);"
)
oled_content = oled_content.replace(
    "int sec = timeStr.length() >= 8 ? timeStr.substring(6, 8).toInt() : 0;",
    "int sec = s_val;"
)
# Style 3
oled_content = oled_content.replace(
    "oled.print(timeStr.length() >= 2 ? timeStr.substring(0, 2) : \"00\");",
    "oled.printf(\"%02d\", h);"
)
oled_content = oled_content.replace(
    "oled.print(timeStr.length() >= 5 ? timeStr.substring(3, 5) : \"00\");",
    "oled.printf(\"%02d\", m);"
)

# Style 4 (Binary)
oled_content = oled_content.replace(
    "oled.print(timeStr.length() >= 8 ? timeStr : \"00:00:00\");",
    "oled.printf(\"%02d:%02d:%02d\", h, m, s_val);"
)
# Remove the substring parsing in Style 4
oled_content = re.sub(
    r'int h = 0, m = 0, sec = 0;\s*if \(timeStr.*?\}',
    'int sec = s_val;',
    oled_content,
    flags=re.DOTALL
)
oled_content = oled_content.replace("int digits[6]", "const int digits[6]")

# Style 5 (Cyberpunk Box)
oled_content = oled_content.replace(
    "oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : \"00:00\");",
    "oled.printf(\"%02d:%02d\", h, m);"
)
oled_content = oled_content.replace(
    "int sec = timeStr.length() >= 8 ? timeStr.substring(6, 8).toInt() : 0;",
    "int sec = s_val;"
)

# Style 6 (Radial Horizon)
oled_content = oled_content.replace(
    "oled.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : \"00:00\");",
    "oled.printf(\"%02d:%02d\", h, m);"
)

# Fix method call inside draw
oled_content = oled_content.replace(
    "drawMode0_HUD(sm.data, ipStr, timeStr, rssi);",
    "drawMode0_HUD(sm.data, ipStr, h, m, s_val, rssi);"
)
oled_content = oled_content.replace(
    "drawMode1_BigClock(timeStr, ipStr, sm.data, clockStyle);",
    "drawMode1_BigClock(h, m, s_val, ipStr, sm.data, clockStyle);"
)

with open("include/display_oled.h", "w") as f:
    f.write(oled_content)

######################
# display_tft.h
######################
with open("include/display_tft.h", "r") as f:
    tft_content = f.read()

# Update renderCurrentPage signature
tft_content = tft_content.replace(
    "void renderCurrentPage(const OutdoorWeatherData& weather, SensorManager& sensors, PomodoroTimer& pomo, const NotificationManager& notif, const String& timeStr, const String& ipStr, int oledMode, int rssi)",
    "void renderCurrentPage(const OutdoorWeatherData& weather, const SensorManager& sensors, const PomodoroTimer& pomo, const NotificationManager& notif, int h, int m, int s_val, const String& ipStr, int oledMode, int rssi)"
)

# Update renderDashboardPage signature and call
tft_content = tft_content.replace(
    "void renderDashboardPage(const String& timeStr",
    "void renderDashboardPage(int h, int m, int s_val"
)
tft_content = tft_content.replace(
    "renderDashboardPage(timeStr",
    "renderDashboardPage(h, m, s_val"
)
tft_content = tft_content.replace(
    "tft.print(timeStr.length() >= 5 ? timeStr.substring(0, 5) : \"00:00\");",
    "tft.printf(\"%02d:%02d\", h, m);"
)
tft_content = tft_content.replace(
    "int sec = timeStr.length() >= 8 ? timeStr.substring(6, 8).toInt() : 0;",
    "int sec = s_val;"
)

# Update renderBigClockPage signature and call
tft_content = tft_content.replace(
    "void renderBigClockPage(const String& timeStr, const OutdoorWeatherData& weather, SensorManager& sensors, PomodoroTimer& pomo, bool fullRedraw)",
    "void renderBigClockPage(int h, int m, int s, const OutdoorWeatherData& weather, const SensorManager& sensors, const PomodoroTimer& pomo, bool fullRedraw)"
)
# Remove time parsing block in renderBigClockPage
tft_content = re.sub(
    r'int h = 0, m = 0, s = 0;\s*if \(timeStr.*?\}',
    '',
    tft_content,
    flags=re.DOTALL
)
tft_content = tft_content.replace(
    "renderBigClockPage(timeStr, weather, sensors, pomo, fullRedraw);",
    "renderBigClockPage(h, m, s_val, weather, sensors, pomo, fullRedraw);"
)

with open("include/display_tft.h", "w") as f:
    f.write(tft_content)


######################
# main.cpp
######################
with open("src/main.cpp", "r") as f:
    main_content = f.read()

# Replace string allocation with integer fetching
main_content = main_content.replace(
    "String timeStr = getFormattedNtpTime();",
    ""
)
main_content = main_content.replace(
    "timeStr = getFormattedNtpTime();",
    """struct tm timeinfo;
        int h = 0, m = 0, s_val = 0;
        if (getLocalTime(&timeinfo)) {
            h = timeinfo.tm_hour;
            m = timeinfo.tm_min;
            s_val = timeinfo.tm_sec;
        }"""
)

# Update method calls
main_content = main_content.replace(
    "oledMgr.draw(sensorMgr, notificationMgr, localIpStr, timeStr, rssi, configMgr.config.oledClockStyle);",
    "oledMgr.draw(sensorMgr, notificationMgr, localIpStr, h, m, s_val, rssi, configMgr.config.oledClockStyle);"
)
main_content = main_content.replace(
    "tftMgr.renderCurrentPage(weatherMgr.weather, sensorMgr, pomoTimer, notificationMgr, timeStr, localIpStr, configMgr.config.oledMode, rssi);",
    "tftMgr.renderCurrentPage(weatherMgr.weather, sensorMgr, pomoTimer, notificationMgr, h, m, s_val, localIpStr, configMgr.config.oledMode, rssi);"
)

with open("src/main.cpp", "w") as f:
    f.write(main_content)

print("Memory leak and string overhead patched successfully.")
