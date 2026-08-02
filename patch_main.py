import re

with open("src/main.cpp", "r") as f:
    c = f.read()

c = c.replace(
    "watchFaceEngine.activeStyle = (watchFaceEngine.activeStyle + 1) % 13;",
    "watchFaceEngine.activeStyle = (watchFaceEngine.activeStyle + 1) % 15;"
)
c = c.replace(
    "configMgr.config.oledClockStyle = (configMgr.config.oledClockStyle + 1) % 12;",
    "configMgr.config.oledClockStyle = (configMgr.config.oledClockStyle + 1) % 14;"
)

with open("src/main.cpp", "w") as f:
    f.write(c)

print("main.cpp updated")
