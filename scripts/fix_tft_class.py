with open("include/display_tft.h", "r") as f:
    c = f.read()

# I will find "};\n\n#endif // DISPLAY_TFT_H" and move it to the end of the file.
c = c.replace("};\n\n#endif // DISPLAY_TFT_H\n", "")
c = c + "\n};\n\n#endif // DISPLAY_TFT_H\n"

with open("include/display_tft.h", "w") as f:
    f.write(c)
print("Class structure fixed!")
