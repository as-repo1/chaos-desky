# ⚡ chaos-desky — ESP32 Smart Dual-Display Command Station

Welcome to **chaos-desky**, an advanced, feature-rich standalone ESP32 dual-display desktop command hub. This project combines dual hardware displays (Color TFT + Monochrome OLED) with an embedded responsive web dashboard, Smartwatch BLE Phone Notification Sync, Custom Watch Face Studio, and Cute Animated Screensavers.

---

## ✨ Key Features

- ⌚ **Smartwatch BLE iPhone Notification Receiver (`ChaosDesky-Watch`)**:
  - Connects as an Apple Notification Center Service (ANCS) BLE client.
  - Automatically receives live **iPhone Phone Calls, WhatsApp messages, iMessages, Gmail, Instagram, and iOS app notifications**!
  - Displays high-contrast popup banners with countdown timers across TFT and OLED screens, just like an Apple Watch.

- 🎨 **Watch Face Studio & Custom Dials (Page 8)**:
  - **Swiss Luxury Analog Dial**: Round dial bezel, 12-hour tick marks, hour/minute hands, sweeping second hand, and live weather complication.
  - **Cyberpunk Dual Chronograph**: Analog dial + digital timestamp, indoor telemetry, and RAM status gauge.
  - **Modern Minimalist Digital Weather**: Large digital clock + OpenWeather condition icon & Hinjewadi (411057) telemetry.
  - **Retro Neon Nixie Tube**: Neon-glowing digits + notification count badge.

- 🐱 **Cute & Symbolic Animated Screensavers**:
  - **OLED Mode 5 (`Cute Cyber Cat Mascot`)**: Frame-by-frame animated pixel cat with happy/wink expressions (`^ _ ^` $\rightarrow$ `- _ -` $\rightarrow$ `^ _ ~`), tail wiggling, and floating heart `♥️` / star `⭐` particles.
  - **TFT Page 10 (`3D Warp Space`)**: 3D Warp Speed Starfield, orbiting cosmic planet & moon, and bouncing **CHAOS** logo!

- 🌊 **Creamy Laser Wipe Screen Transitions & 60 FPS Sub-Pixel Ticker**:
  - Laser scan wipe line sweeps vertically across the TFT screen during page switches for buttery smooth, flicker-free transitions.
  - Smooth 1-pixel sub-pixel scrolling ticker on the OLED marquee.

- 🌐 **Clutter-Free Tabbed Web Portal**:
  - Reorganized into 5 clean, easy-to-navigate tabs:
    - 📊 **Telemetry**: Indoor climate HUD & Hinjewadi 411057 OpenWeather stats.
    - ⌚ **Watch Faces & Displays**: Watch Face selector, 11 Theme palettes, Screen Rotation, OLED Modes.
    - 🔔 **Smartwatch & Popups**: iPhone BLE Sync state, Test Phone Call Popup, Live Display Alert Sender.
    - 📝 **Custom Media**: Custom Text Banner Publisher & HTML5 Image Converter.
    - ⚙️ **Settings**: Wi-Fi, OpenWeather API Key, 10-Page Carousel Bitmask, Pomodoro Sliders.

- 🖼️ **HTML5 Canvas Custom Image Uploader**:
  - Web UI converts user photos into 16-bit RGB565 for TFT (Page 5) or 1-bit monochrome bitmap for OLED (Mode 4).

- 🎨 **11 Design System Color Themes**:
  - Cyberpunk, Matrix, Dark Glass, Retro, Dracula, Nord, Gruvbox, Monochrome, Nothing UI, One UI, Material You.

- 📱 **Default 180° Inverted TFT Display Rotation**:
  - Default hardware rotation configured to 180° (`setRotation(2)`).

---

## 🛠️ Hardware Requirements

| Component | Description | Protocol / Pins |
| :--- | :--- | :--- |
| **Microcontroller** | ESP32 Dev Module (WROOM-32 / ESP32-D0WD) | 240MHz, 320KB RAM |
| **Primary Display** | 1.8" SPI Color TFT LCD (ST7735, 128x160) | SPI (`CS: 15, DC: 16, RST: 17, MOSI: 13, SCK: 14`) |
| **Secondary Display** | 0.96" I2C Monochrome OLED (SSD1306, 128x64) | I2C (`SDA: 21, SCL: 22`) |
| **Climate Sensor** | DHT11 (Digital Temp & Humidity) | Digital IO (`GPIO 4`) |
| **Pressure Sensor** | BMP180 or BMP280 (Barometric Pressure & Altitude) | I2C (`SDA: 21, SCL: 22`) |

---

## 🔌 Circuit Diagram

```mermaid
graph TD
    ESP32["ESP32 Microcontroller"]

    subgraph I2C_Bus ["I2C Bus (Shared)"]
        ESP32 -- "GPIO 21 (SDA)" --> OLED["0.96 inch OLED SSD1306"]
        ESP32 -- "GPIO 22 (SCL)" --> OLED
        ESP32 -- "GPIO 21 (SDA)" --> BMP["BMP180/280 Sensor"]
        ESP32 -- "GPIO 22 (SCL)" --> BMP
    end

    subgraph SPI_Bus ["SPI Bus (Dedicated)"]
        ESP32 -- "GPIO 13 (MOSI)" --> TFT["1.8 inch Color TFT ST7735"]
        ESP32 -- "GPIO 14 (SCK)"  --> TFT
        ESP32 -- "GPIO 15 (CS)"   --> TFT
        ESP32 -- "GPIO 16 (DC)"   --> TFT
        ESP32 -- "GPIO 17 (RST)"  --> TFT
    end

    subgraph Digital_IO ["Digital IO"]
        ESP32 -- "GPIO 4 (DATA)"  --> DHT["DHT11 Sensor"]
    end

    subgraph Power_Dist ["Power Distribution"]
        3V3["3.3V Power"] --> OLED
        3V3 --> TFT
        3V3 --> DHT
        3V3 --> BMP
        GND["Ground"]     --> OLED
        GND --> TFT
        GND --> DHT
        GND --> BMP
    end
```

---

## 🚀 Quick Start Guide

1. Clone repository:
   ```bash
   git clone https://github.com/chaos/chaos-desky.git
   cd chaos-desky
   ```
2. Build & Flash Firmware + LittleFS Filesystem:
   ```bash
   pio run -t upload && pio run -t uploadfs
   ```
3. Open Web Dashboard:
   - Connect to `http://<ESP32_IP>` or `http://192.168.1.6`.
