# 📜 Changelog — `chaos-desky`

All notable changes to the **chaos-desky** project will be documented in this file.

---

## [1.5.0] - 2026-07-27

### ✨ Added
- **⌚ Watch Face Studio Engine (`include/watchface_engine.h`)**:
  - **Classic Swiss Luxury Analog Dial**: Round bezel, 12-hour tick marks, hour/minute/sweeping second hands, weather badge.
  - **Cyberpunk Dual Chronograph**: Analog dial + digital timestamp, indoor telemetry, RAM status gauge.
  - **Modern Minimalist Digital Weather**: Big digital clock + OpenWeather condition icon & location stats.
  - **Retro Neon Nixie Tube**: Bold nixie digits + notification count badge.
- **📱 Smartwatch BLE iPhone Notification Receiver (`include/ancs_client.h`)**:
  - Connects over BLE as ANCS GATT client (`ChaosDesky-Watch`).
  - Automatically receives live **iPhone Phone Calls, WhatsApp messages, iMessages, and iOS alerts** with auto-dismiss popup overlays!
- **🐱 Cute & Symbolic Animated Screensavers (`include/screensaver.h`)**:
  - OLED Mode 5: Cute Cyber Cat Mascot with blinking eyes (`^ _ ^` $\rightarrow$ `- _ -` $\rightarrow$ `^ _ ~`), tail wiggling, and floating heart/star particles.
  - TFT Page 10: 3D Warp Speed Starfield, orbiting cosmic planet & moon, and bouncing CHAOS logo.
- **🌊 Creamy Laser Scan Wipe Page Transitions**:
  - Laser scan wipe line sweeps across the TFT screen during page switches for buttery smooth, flicker-free transitions.
- **🌐 Clutter-Free Tabbed Web Portal**:
  - Reorganized dashboard into 5 clean tabbed panes: Telemetry, Watch Faces, Smartwatch, Media, and Settings.
- **🔄 Default 180° Inverted TFT Display Rotation**:
  - Configured default TFT rotation to 180° (`setRotation(2)`).

---

## [1.1.0] - 2026-07-27

### ✨ Added
- **11 Design System Themes** (Cyberpunk, Matrix, Dark Glass, Retro, Dracula, Nord, Gruvbox, Monochrome, Nothing UI, One UI, Material You).
- **🖼️ HTML5 Canvas Image Uploader** for TFT & OLED displays.
- **💾 Persistent Configuration Engine (`/config.json`)** on LittleFS.

---

## [1.0.0] - 2026-07-27

### ✨ Initial Release
- Standalone ESP32 Firmware with Dual-Display Architecture (SSD1306 OLED + ST7735 TFT).
- OpenWeatherMap API integration, Zambretti Barometric Storm Predictor, and AsyncWebServer dashboard.
