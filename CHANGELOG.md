# 📜 Changelog — `chaos-desky`

All notable changes to the **chaos-desky** project will be documented in this file.

---

## [2.3.0] - 2026-07-28 (Watchface Studio Precision Fine-Tuning & 3 New Iconic Faces)

### ✨ Added & Enhanced
- **🎨 Precision Fine-Tuned Casio Watchfaces**:
  - **Casio F-91W**: Added authentic gold/blue dual pinstripe borders, `SU MO TU WE TH FR SA` LCD day indicators, authentic 24H indicator, and temperature badge.
  - **Casio G-Shock**: Upgraded DW-5600 octagonal resin bezel frame with 3 tactical sub-dials (`TMP`, `OUT`, `WND`) and high-contrast digital time.
  - **Casio CA-53W Calculator**: Added 3D embossed keypad with math operator keys (`+`, `-`, `x`, `÷`, `=`) and top LCD calculation window.
  - **Casio Royale AE-1200**: Fine-tuned radar sub-dial with dynamic rotating radar sweep line, latitude/longitude grid, world time zone list, and 10-year battery indicator.
- **⌚ 3 Brand-New Watchface Styles (Total: 13 Watchfaces)**:
  - **Style 10: Casio A168 Vintage ElectroLuminescence**: Retro silver/gold watch face with full-panel cyan-blue **EL backlight glow**, `ILLUMINATOR` text, and `WATER RESIST 50M`.
  - **Style 11: Casio DB-360 Databank Telememo 30**: Iconic multi-line LCD screen with `TELEMEMO 30` header, memory capacity usage bar, contact info preview, and dual time.
  - **Style 12: Cyberpunk 2077 Night City Chrono**: High-tech Cyberpunk HUD watchface with glowing neon yellow/cyan angular frame, CPU telemetry progress bar, and glitching neon time digits.

---

## [2.2.0] - 2026-07-28 (TFT Page 10: OLED Studio Hub & Direct Hardware Switching)

### ✨ Added & Enhanced
- **📺 Dedicated TFT OLED Controller (Page 10, Index 9)**: Added a brand new interactive TFT dashboard screen titled **"OLED Studio Hub"** displaying real-time OLED broadcast mode status and active clock face style.
- **🔘 Dedicated Right Switch OLED Actions**:
  - **Single Click on Page 10**: Directly cycles through all 6 OLED display modes (Telemetry HUD -> Clock -> Sparklines -> Marquee Ticker -> Screensaver -> WiFi Spec).
  - **Double Click on Page 10**: Cycles through the 8 OLED Clock Face styles when the dynamic clock is active.
- **🔄 Auto-Migration & Web Portal Sync**: Automatically upgraded `enabledPagesMask` to 10 bits (`0x03FF`) with intelligent migration logic so existing users immediately gain Page 10 without clearing EEPROM. Updated Web UI with P10 instant jump and filter toggles.

---

## [2.1.0] - 2026-07-28 (Comprehensive Bug Purge & Interactive To-Do Sync)

### ✨ Added & Enhanced
- **📝 Interactive To-Do List Editor in Web Portal**: Added dedicated dashboard card to view, edit, check, and sync the 4 daily tasks in real time between the web app and the TFT display.
- **💾 Task State & Checkbox Persistence**: All task titles and completion states are now fully serialized and persisted to `config.json` via LittleFS.

### 🐛 Fixed & Optimized
- **⌚ Fixed Watchface Studio Navigation Jump**: Corrected API route (`/api/tft/watchface`) that mistakenly navigated to Page 8 (Network Monitor) instead of Page 7 (Watchface Studio) when selecting watch styles online.
- **🕹️ Flappy Bird Purge & Carousel Normalization**: Excluded all redundant Flappy Bird game code and normalized `TOTAL_TFT_PAGES` to 9 across both firmware and frontend JavaScript masks, removing CPU polling overhead.
- **🌐 Web Dashboard Script Resolution**: Removed 100+ lines of redundant inline JS in `index.html` that collided with `app.js` logic and resolved broken endpoint targets for Pomodoro and Weather location persistence.
- **🔘 Left Button Long-Hold Jump**: Removed blocking debouncer guard on button release, restoring instant jump to Home (Page 1) when long-pressing the Left navigation button.

---

## [2.0.0] - 2026-07-27 (The Precision Optimization & Dedicated UX Update)

### ✨ Overhauled & Optimized
- **🕹️ Dedicated Separation of Switch Duties (Left = Navigation | Right = Page Action)**:
  - Streamlined mechanical key ergonomics: **Left Button (D25)** is exclusively reserved for page and screen navigation (Single Click for next TFT page, Double Click for previous page, Long Hold for immediate home jump to Page 0).
  - **Right Button (D26)** serves as the dedicated functional execution button for whatever page is currently displayed on the screen:
    - **Weather & Climate Pages**: Instant cloud API refresh and hardware sensor read triggers.
    - **Pomodoro Timer**: Single Click Toggles Start/Pause; Double Click Resets the timer back to 25:00.
    - **To-Do Task Board**: Single Click toggles checkmark state; Double Click moves focus down the list.
    - **Watchface Studio**: Single Click cycles the 10 watch styles; Double Click applies the iconic Casio F-91W.
    - **System & Hardware Pages**: Quick toggles for OLED display modes and cycling TFT color themes.
    - **Global Long Hold**: Instantly unleashes the animated dual screensavers across both displays!
- **⚡ Surgical Flash Memory Optimization**:
  - Successfully reduced firmware footprint from 102.5% overflow down to an optimal **98.1%** flash memory utilization without sacrificing high-frequency animations or custom aesthetics!
  - Removed unused ANCS phone notification structs and replaced overhead-heavy macro tables with clean, compiled switch statements.
  - Streamlined TFT presentation from 10 pages down to 9 precision-tuned pages and OLED presentation to 6 distinct modes.
- **⏱️ Pomodoro Pause State Retention**:
  - Resolved an issue where resuming a paused Pomodoro timer would incorrectly transition to an IDLE state instead of resuming the exact countdown from its paused working state (`POMO_WORK` vs `POMO_BREAK`).

---

## [1.8.1] - 2026-07-27

### ✨ Added & Enhanced
- **⚙️ Customizable System & Alert Notification Display Preference (`/api/notify/target`)**:
  - Added real-time selector to Web Portal (Card 4) allowing users to decide where all system change alerts, BLE notifications, timers, and theme switch popups are routed (**TFT Only**, **OLED Only**, or **Both Displays**).
  - Implemented automatic LittleFS persistence via `notifTarget` in `SystemConfig` and added `NOTIF_TARGET_USER_PREF` across all core notification triggers in firmware.
- **🔄 Re-Aligned Mechanical Button Default Macros**:
  - **Left Switch (D25) Single Click**: Switches between pages directly on the **TFT Color Screen** (`ACT_NEXT_TFT_PAGE`).
  - **Right Switch (D26) Single Click**: Cycles through modes on the **OLED Display** (`ACT_CYCLE_OLED`).
  - **Left Switch Double Click**: Enters interactive **To-Do & Notes Board** functionality (`ACT_JUMP_TODO`).
  - **Right Switch Double Click**: Enters **Watchface Studio** functionality (`ACT_JUMP_WATCH`).

---

## [1.8.0] - 2026-07-27

### ✨ Added & Overhauled
- **🎛️ Fully Customizable Macro & Button Studio (`/api/buttons/config`)**:
  - Re-mapped mechanical key functionality from hardcoded loops into a clean, event-driven dispatcher (`executeButtonAction()`).
  - Added full web dashboard customizer (Card 9) allowing real-time assignment of Single Click, Double Click, and Long Hold events for both Left (D25) and Right (D26) keys with automatic LittleFS persistence.
- **🤝 Simultaneous Dual-Button Combo Detection (`DualSwitchComboDetector`)**:
  - Implemented an 80ms coincidence timing window in `mech_switch.h` to cleanly recognize simultaneous button presses without delaying individual click events.
  - Configurable global super-macros including instant **WiFi Credentials & QR Broadcast** and stealth dual screensaver activation.
- **🧠 Smart Context-Aware Controls**:
  - Button clicks automatically adapt based on active screen context. On interactive pages (To-Do List & Pomodoro Timer), clicks effortlessly transform into item selectors, live task completion checkboxes, and timer control shortcuts.
- **📟 8 Iconic OLED Clock Styles (`display_oled.h`)**:
  - Expanded OLED multi-face clock options from 6 to 8 styles by adding **Style 6: Cyberpunk Boxed Frame** and **Style 7: Radial Horizon Arc Clock**.
- **⚡ Ultra-Fast Fluid OLED Marquee Announcement Speed**:
  - Upgraded custom text scrolling ticker to an ultra-responsive **45ms** update frequency with an 8-pixel per frame advancement rate, providing lightning-fast, smooth banner animations.
- **📚 Complete Documentation Refresh**:
  - Comprehensive updates across `README.md`, `README_ARCHITECTURE.md`, and `README_PIN_DIAGRAM.md` reflecting the new Macro Engine, hardware schematics, and UI capabilities.

---

## [1.7.5] - 2026-07-27

### ✨ Added & Enhanced
- **⌚ 10 Iconic TFT Watch Faces (`watchface_engine.h`)**:
  - Swiss Analog, Cyber Chrono, Modern Digital, Neon Nixie, Casio F-91W, Casio G-Shock, **Casio CA-53W Calculator**, **Casio Royale (AE-1200 World Time Radar)**, **Seiko Diver (Luminescent Marine)**, and **Pulsar LED (1972 Ruby Red)**.
- **📟 6 Multi-Style OLED Clock Faces (`display_oled.h`)**:
  - Digital Telemetry HUD, Analog Minimalist Dial, Cyber Matrix HUD, Retro Airport Flip Cards, Oversized Vertical Stack, and Binary Segment Gauges!
- **⚡ Pixel-Perfect Alignment Overhaul**:
  - Re-calculated character bounding boxes ($6\text{px}$ Size 1, $12\text{px}$ Size 2, $18\text{px}$ Size 3) across all watchfaces and dashboard screens to eliminate digit overlaps and text spillage.
- **🚀 50px/sec OLED Marquee Ticker**:
  - Accelerated custom announcement ticker scrolling speed to 50px/sec at a 100ms refresh rate for fluid, rapid message broadcasting.
- **🧹 Image Uploader Purge & Page Refocus**:
  - Removed photo canvas uploader and memory buffers. Refocused TFT Page 5 as an interactive **To-Do List Dashboard** and OLED Mode 4 as a **Retro Cyber Radar Graphic**.

---

## [1.7.0] - 2026-07-27

### ✨ Added & Modified
- **⌨️ Static Screen Dual-Switch Hardware Hub (`MECH_SWITCH_1_PIN = 25`, `MECH_SWITCH_2_PIN = 26`)**:
  - Both screens are completely static by default (`carouselSpeedSec = 0`). Auto-slideshow cycling is completely disabled so screens only transition on your explicit physical switch presses!
  - **Left Key (Pin D25 ➔ GND) — "OLED Controls & To-Do / Notes Quick-Jump"**:
    - *Single Click*: Advances pages/modes directly on the **OLED Display** (HUD ➔ Big Clock ➔ Sparkline ➔ Marquee ➔ Cyber Cat).
    - *Double Click*: Instant toggle jump between the **To-Do List (Page 4)** and **Notes & Logs (Page 6)** screens on the TFT!
    - *Long Press (>700ms)*: Quick-jumps to Pomodoro screen & toggles work countdown!
  - **Right Key (Pin D26 ➔ GND) — "TFT Navigation & Watchface Studio Sync"**:
    - *Single Click*: Manually advances pages directly on the **TFT Display**!
    - *Double Click*: Switches OLED display to the **Big Clock Face**, snaps TFT to Watchface Studio, and cycles between iconic watch faces (Swiss, Nixie, **Casio F-91W**, and **Casio G-Shock**)!
    - *Long Press (>700ms)*: Hotkey to snap straight to your Casio F-91W watchface and cycle themes!
- **⚡ Resource & Performance Optimization Hub (Web UI Card 10 & API)**:
  - Added interactive feature toggles in the Web UI to dynamically enable/disable resource-intensive subsystem engines via `/api/optimize`:
    - **BLE UART Radio & Telemetry (`featureBleEnabled`)**: Stops RF broadcasts and saves ~50KB RAM.
    - **OpenWeatherMap HTTPS Polling (`featureWeatherEnabled`)**: Disables regular TLS requests to eliminate network packet spikes.
    - **TFT Cyber-Warp Screensaver Math (`featureScreensaverEnabled`)**: Skips floating point particle kinematics when idling.
- **🖥️ Web UI Polish & Static Routing**:
  - Updated Hardware Hub instructions in Web UI Card 9 and renamed Page 4 to `TO-DO LIST` and Page 6 to `NOTES & LOG` for crystal-clear readability!
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
