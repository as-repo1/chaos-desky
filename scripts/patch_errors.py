import re

with open("include/display_tft.h", "r") as f:
    content = f.read()

content = content.replace("renderBigClockPage(timeStr, weather, sensors, pomoTimer, fullRedraw);", "renderBigClockPage(timeStr, weather, sensors, pomo, fullRedraw);")

with open("include/display_tft.h", "w") as f:
    f.write(content)

with open("include/watchface_engine.h", "r") as f:
    content = f.read()

content = content.replace("sensors.data.pressureHPa", "sensors.data.pressureHpa")
content = content.replace("pomo.isActive", "(pomo.state != POMO_IDLE)")
content = content.replace("pomo.isWorkPhase", "(pomo.state == POMO_WORK)")
content = content.replace("pomo.timeLeft", "pomo.remainingSec")
content = content.replace("pomo.totalPhaseTime", "pomo.durationSec")
content = content.replace("w.temp", "w.tempC")
content = content.replace("w.sensors.data.tempC", "w.tempC")

# Fix multiple replaced tempC from w.tempC -> w.tempCC
content = content.replace("w.tempCC", "w.tempC")

with open("include/watchface_engine.h", "w") as f:
    f.write(content)

print("Errors patched.")
