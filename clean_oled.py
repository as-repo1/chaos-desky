import re

with open("include/display_oled.h", "r") as f:
    content = f.read()

# Remove everything from the broken line onwards
idx = content.find("const int digits[6] = { h / 10, h % 10, m / 10, m % 10, sec / 10, sec % 10 };")
if idx == -1:
    print("Cannot find anchor")
else:
    # Keep up to the end of BinaryGauges
    idx2 = content.find("    // --- OLED Clock Style 6: Cyberpunk Box ---", idx)
    # Actually wait, Binary Gauges is style 5. Then Cyberpunk Box is style 6. Then Radial Horizon is style 7.
    # Where was Orbit injected? Right inside BinaryGauges?
    pass
