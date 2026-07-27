# 📜 Changelog — `chaos-desky`

All notable changes to the **chaos-desky** project will be documented in this file.

---

## [1.7.0] - 2026-07-27

### ✨ Added & Modified
- **⌨️ Dual Bare Mechanical Switch Hardware Controls (`MECH_SWITCH_1_PIN = 25`, `MECH_SWITCH_2_PIN = 26`)**:
  - Both pins use internal pull-up resistors (`INPUT_PULLUP`) with 35ms software debounce—**zero external resistors or capacitors required!**
  - **Switch 1 (Pin D25 ➔ GND) — "Navigation & Slideshow Toggle"**:
    - *Single Click*: Manually advances to Next TFT Dashboard Page (and resets auto-slideshow timer if running).
    - *Double Click*: Toggles between **Manual Navigation Only** and **Auto-Slideshow Mode** (6 seconds per slide), complete with a live dual-screen popup notification!
    - *Long Press (>700ms)*: Cycles through OLED Display Modes (HUD ➔ Big Clock ➔ Sparkline ➔ Marquee ➔ Cyber Cat Screensaver)!
  - **Switch 2 (Pin D26 ➔ GND) — "Interactive Control & Actions"**:
    - *Single Click*: Interacts directly with Whatever screen is displayed!
      - On **Watchface Page (P8)**: Cycles all 6 Watch Faces (including Casio F-91W & G-Shock).
      - On **Pomodoro Page (P3)**: Instantly Starts / Pauses the countdown timer.
      - On **Dashboard Pages**: Cycles through all 11 TFT Color Themes (Cyberpunk Neon, Matrix, Dracula, Nord Frost, etc.)!
    - *Double Click*: Quick-jumps to the Pomodoro Timer screen and toggles Start/Pause!
    - *Long Press (>700ms)*: Instant hotkey that snaps straight to your iconic **Casio F-91W** watchface!
- **🛑 Manual-Only Navigation Default**:
  - Changed default `carouselSpeedSec` to `0` so the device boots in pure manual navigation mode without unexpected screen transitions.
- **🖥️ Web UI Documentation Update**:
  - Updated Hardware Card 9 on the Web UI (`index.html`) with interactive pin diagrams and instructions for both D25 and D26 switches.

---

## [1.6.0] - 2026-07-27

### ✨ Added
- **⌚ Iconic Casio Watch Faces (`include/watchface_engine.h`)**:
  - **Casio F-91W**: Authentic blue header bar, red watermark accents, AL/CH badges, day/date block, digital LCD font styling, indoor & outdoor temperature readouts, and classic 3-button layout indicators!
  - **Casio G-Shock Databank / Protec**: Tough armor bezel styling, dual digital time/date arrays, environmental monitoring gauges, and active security protection status!
- **⌨️ Bare Mechanical Keyboard Switch Support (`include/mech_switch.h`)**:
  - Works with a single physical switch wired directly between **GPIO 25** and **GND** (zero external resistors or capacitors required, powered by `INPUT_PULLUP`).
  - **35ms Software Debounce Filter**: Clean button event detection without hardware debounce caps.
  - **Single Click**: Cycles through all 6 Watch Faces on the Watchface page, or jumps to the next TFT page on dashboard views!
  - **Double Click**: Toggles Pomodoro timer (Start/Pause) and snaps directly to the Pomodoro screen!
  - **Long Press (>700ms)**: Instant hotkey that immediately summons the iconic Casio F-91W watchface!
- **📡 Open BLE Nordic UART & PC Telemetry Receiver (`include/ble_uart_server.h`)**:
  - Replaced ANCS pairing loops with open Nordic UART Service (`6E400001-B5A3-F393-E0A9-E50E24DCCA9E`).
  - Works instantly with **any iOS/Android phone or PC script** without encryption bonding failures!
  - **Wireless Commands**: Type `casio` or `gshock` to change watchfaces, `page <N>` to flip dashboard screens, `status` to retrieve sensor telemetry, or `pc:<stats>` to broadcast real-time PC CPU/GPU gaming stats directly onto both OLED and TFT displays!

### 🔧 Fixed & Optimized
- **🚫 Zero-Flicker Graphics & OLED Overdraw Elimination**:
  - Rewrote rendering engines across both displays to eliminate disruptive background clearing and flickering.
  - **OLED (`display_oled.h`)**: Added strict dirty state tracking (`needsRefresh`). Big Clock, sensor gauges, and marquees only perform I2C writes when timestamps or data values actually change.
  - **TFT (`display_tft.h`, `watchface_engine.h`)**: Added `fullRedraw` background protection and incremental digit updating. Dials, frames, and watchface backdrops render once, while dynamic indicators paint smoothly without full-screen flash.
- **✨ Polished Modern Web UI & Design Aesthetics (`data/index.html`, `data/style.css`)**:
  - Enhanced modern dark glassmorphism styling, smooth button micro-interactions, active depression states, and expanded watchface studio controls.

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
