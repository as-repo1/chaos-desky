import re

with open("include/display_oled.h", "r") as f:
    content = f.read()

# Fix the broken array initialization
broken_line = "const int digits[6] = { h / 10, h % 10, m / 10, m % 10, sec / 10, sec % 10     // --- OLED Clock Style 8: Orbit ---"
fixed_part = "const int digits[6] = { h / 10, h % 10, m / 10, m % 10, sec / 10, sec % 10 };"

content = content.replace(broken_line, fixed_part + "\n    // --- OLED Clock Style 8: Orbit ---")

# And re-add }; at the end since we removed it
content += "\n};\n"

with open("include/display_oled.h", "w") as f:
    f.write(content)

print("Fixed!")
