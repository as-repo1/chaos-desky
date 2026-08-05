# ⚡ chaos-desky — ESP32 Dual-Display Desktop & Command Hub

**chaos-desky** is an advanced standalone ESP32 dual-display desktop companion and tactical workstation hub. It combines a vibrant 1.8" ST7735 color TFT screen, an ultra-crisp 0.96" SSD1306 OLED display, dual mechanical keyboard switches, environmental sensors (DHT11 & BMP180), multi-brand watch faces, animated screensavers, and an embedded LittleFS interactive web dashboard.

---

## 🛠️ Key Features & Capabilities

- 🖥️ **Dual Display Command Center**:
  - **1.8" SPI Color TFT LCD (ST7735, 128x160)**: 13-page custom carousel display:
    1. **Outdoor Weather**: Live temperature, weather icon, condition, wind & humidity from OpenWeatherMap.
    2. **Barometric Pressure Trend & Zambretti Forecaster**: Sparkline graph & local weather prediction.
    3. **Pomodoro Productivity Timer**: Interactive timer with pause/resume and visual countdown ring.
    4. **System QR Code Dashboard**: QR code to web portal, WiFi IP & Heap memory status.
    5. **To-Do List & Daily Focus Board**: Interactive checklist with live checkoff controls.
    6. **Indoor Climate Telemetry**: Temperature, Dew Point, Heat Index, Humidity, Altitude — **7 UI themes**.
    7. **Watch Face Studio**: 15 Iconic Watch Faces including analog and digital classics.
    8. **Network Monitor**: Live WiFi signal, RSSI, MAC address & gateway stats.
    9. **System Hardware Diagnostics**: Free Heap RAM, Flash utilization, uptime, CPU status.
    10. **OLED Studio Hub**: Dedicated TFT controller to manage OLED modes and clock faces.
    11. **Temperature History Graph**: Historical line chart tracking temperature trends.
    12. **Humidity History Graph**: Historical line chart tracking ambient humidity.
    13. **Comfort Index**: Visual air quality and comfort level indicator.
  - **0.96" I2C Monochrome OLED (SSD1306, 128x64)**: 6 distinct operational modes:
    0. **Telemetry HUD**: Live clock, IP address, Temperature & WiFi RSSI.
    1. **Multi-Style Clock Faces**: Digital HUD, Analog Minimalist, Cyber Matrix, Retro Airport Flip, Vertical Stack, Binary Gauges, Cyberpunk Frame, Radial Horizon Arc!
    2. **Sparkline Telemetry**: Historical Barometric pressure and indoor climate trend lines.
    3. **Fast-Speed Marquee Ticker**: 45ms scrolling custom desk notes and announcements.
    4. **Animated Screensaver**: Matrix Code Rain, Bouncing DVD Logo, 3D Tunnel, DNA Helix, Batman Signal, Linux Tux!
    5. **WiFi Credentials Broadcast**: Network status, IP, SSID, and connection instructions.

- 🕹️ **Dual Mechanical Switch Deck (Left = Nav | Right = Action)**:
  - GPIO 25 (Left Key) and GPIO 26 (Right Key) to GND via internal pullups. No external resistors!
  - **⬅️ Left Button**:
    - **Single Click**: Instantly cycles forward through the 13 TFT pages.
    - **Double Click**: Cycles backward through pages.
    - **Long Press**: Jump home to Page 0 (Weather).
  - **➡️ Right Button (Page-Specific)**:
    - Page 0 (Weather): Force instant OpenWeatherMap refresh.
    - Page 1 (Barometer): Log pressure sample to trend graph.
    - Page 2 (Pomodoro): Toggle Start/Pause; Double click resets to 25:00.
    - Page 3 (System QR): Cycle OLED display mode.
    - Page 4 (To-Do): Check/Uncheck task; Double click moves focus down.
    - Page 5 (Climate): Cycle 7 climate UI themes.
    - Page 6 (Watchface): Cycle 15 watch styles; Double click jumps to Casio F-91W.
    - Page 7 (Network): Broadcast WiFi credentials across both screens.
    - Page 8 (Hardware): Cycle 11 TFT Color Themes.
    - Page 9 (OLED Hub): Cycle OLED modes; Double click cycles clock face styles.
    - **Long Press (Global)**: Launch synchronized dual screensaver.
  - **🤝 Dual-Button Combo**:
    - **Short Press**: Broadcast IP address and QR code across both screens.
    - **Long Press (1.5s)**: Cycle the OLED display mode.

- 🌐 **Embedded LittleFS Web Command Portal**:
  - Glassmorphic mobile-responsive UI: Sensor Telemetry, Watch Face Customizer, Pomodoro & To-Do Manager, Wi-Fi Setup, Marquee Publisher, Resource Hub.
  - Real-time sliders for OLED brightness, 11 TFT themes, rotation, page masking, notification targets, and feature toggling.

- ⚡ **Flash Optimized**: `CORE_DEBUG_LEVEL=0` in production. ~97.6% flash (1,918,544 / 1,966,080 bytes).

- 🎨 **11 Color Themes**: Cyberpunk, Matrix, Dark Glass, Retro, Dracula, Nord, Gruvbox, Monochrome, Nothing UI, One UI, Material You.

---

## 🔌 Hardware Wiring

| Component | Interface | ESP32 GPIO Pins | Notes |
| :--- | :--- | :--- | :--- |
| **OLED Display (SSD1306)** | I2C | SDA: `GPIO 21`, SCL: `GPIO 22` | Shared I2C Bus, 3.3V |
| **BMP180 Sensor** | I2C | SDA: `GPIO 21`, SCL: `GPIO 22` | Shared I2C Bus, 3.3V |
| **DHT11 Sensor** | Digital | Data: `GPIO 4` | — |
| **TFT Display (ST7735)** | SPI | CS:`15`, DC:`16`, RST:`17`, MOSI:`13`, SCK:`14` | Dedicated SPI Bus |
| **Left Mechanical Switch** | Digital (Pull-Up) | `GPIO 25` ➔ GND | No resistors needed |
| **Right Mechanical Switch** | Digital (Pull-Up) | `GPIO 26` ➔ GND | No resistors needed |

---

## 🚀 Building, Flashing & Running

```bash
# Flash firmware
pio run -t upload

# Upload web dashboard (LittleFS)
pio run -t uploadfs
```

Navigate to `http://<ESP32_IP>` to access the web control portal.
