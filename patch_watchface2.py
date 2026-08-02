import re

with open("include/watchface_engine.h", "r") as f:
    content = f.read()

# Replace `float tempC` in signatures with `SensorManager& sensors, PomodoroTimer& pomo`
content = re.sub(
    r'const OutdoorWeatherData& (w|weather),\s*float tempC,',
    r'const OutdoorWeatherData& \1, SensorManager& sensors, PomodoroTimer& pomo,',
    content
)

# Replace usage of `tempC` with `sensors.data.tempC`
# Since we replaced the parameter, any remaining `tempC` in the body should be updated.
# Be careful not to replace `sensors.data.tempC` to `sensors.data.sensors.data.tempC`
content = content.replace("tempC", "sensors.data.tempC")
# Fix if there are duplicated ones (e.g. if one was already sensors.data.tempC)
content = content.replace("sensors.data.sensors.data.tempC", "sensors.data.tempC")
content = content.replace("float sensors.data.tempC", "float tempC") # Revert any accidental match (though we shouldn't have any left)

with open("include/watchface_engine.h", "w") as f:
    f.write(content)

print("Signatures fixed.")
