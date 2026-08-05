import re

with open("include/watchface_engine.h", "r") as f:
    c = f.read()

c = c.replace(
    """            case WATCHFACE_CYBERPUNK_2077:
                renderCyberpunk2077(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            default:""",
    """            case WATCHFACE_CYBERPUNK_2077:
                renderCyberpunk2077(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_CHRONOGRAPH:
                renderChronograph(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            case WATCHFACE_BAUHAUS:
                renderBauhaus(gfx, hours, mins, secs, weather, sensors, pomo, colorPrimary, colorAccent, colorText, colorBg, fullRedraw);
                break;
            default:"""
)

with open("include/watchface_engine.h", "w") as f:
    f.write(c)
print("Switch updated")
