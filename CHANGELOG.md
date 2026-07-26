# 📜 Changelog — `chaos-desky`

All notable changes to the **chaos-desky** project will be documented in this file.

---

## [1.1.0] - 2026-07-27

### ✨ Added
- **11 Design System Themes** (TFT & Web UI Synchronized):
  - `0`: Cyberpunk Synthwave (Neon Cyan/Magenta)
  - `1`: Matrix Hacker (Terminal Green)
  - `2`: Dark Glass (Charcoal Slate)
  - `3`: Retro Arcade (8-Bit Yellow/Red)
  - `4`: Dracula Theme (Purple/Pink)
  - `5`: Nord Arctic (Frost Pastels)
  - `6`: Gruvbox Retro (Earthy Amber/Aqua)
  - `7`: Monochrome High-Contrast (Stark White/Black)
  - `8`: Nothing UI (Dot-Matrix Red/White)
  - `9`: One UI (Samsung Blue)
  - `10`: Material You (Google Lavender)
- **🖼️ HTML5 Canvas Image Uploader**:
  - Web portal converts any image file into 16-bit RGB565 binary for TFT or 1-bit bitmap for OLED and flashes it to LittleFS storage.
- **💾 Persistent Configuration Engine (`/config.json`)**:
  - Automatically loads and saves all Wi-Fi, OpenWeather API, OLED mode, TFT rotation, custom text, and Pomodoro configurations across reboots.
- **🖥️ Granular OLED Modes & Controls**:
  - 5 Modes: Telemetry HUD, Big Clock & Date, Temperature/Humidity Sparklines, Custom Text Marquee, Uploaded 128x64 Bitmap.
  - Live Contrast Slider (0-255) & Screen Inversion Toggle.
- **📺 Granular TFT Customizer**:
  - Added Page 4 Custom Media Page (renders uploaded custom image or custom text banner).
  - TFT Screen Rotation toggle (0°, 90°, 180°, 270°).
  - Page Bitmask filtering (enable/disable specific pages).
- **⏱️ Custom Pomodoro Sliders**:
  - Web portal sliders for custom Work (1-120m) and Break (1-60m) durations.

---

## [1.0.0] - 2026-07-27

### ✨ Initial Release
- Standalone ESP32 Firmware with Dual-Display Architecture (SSD1306 OLED + ST7735 TFT).
- OpenWeatherMap API integration, Zambretti Barometric Storm Predictor, and AsyncWebServer dashboard.
