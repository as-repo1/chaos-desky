import re

with open("include/watchface_engine.h", "r") as f:
    content = f.read()

# Replace signatures
content = content.replace("SensorManager& sensors, PomodoroTimer& pomo", "const SensorManager& sensors, const PomodoroTimer& pomo")

# Replace arrays
content = content.replace("int vals[12] =", "const int vals[12] =")

with open("include/watchface_engine.h", "w") as f:
    f.write(content)

print("watchface_engine refactored")
