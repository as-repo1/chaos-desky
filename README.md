# ⚡ CHAOSDESKY — The Silicon Oracle & Reality Synthesizer

> *"Chaos is not the absence of order; it is the raw, uncarved marble of creation waiting for a dual-core mind to give it form."*

---

## 👁️ The Manifesto: Philosophy, Mythos & Machine

```
              ┌─────────────────────────────────────────┐
              │    JANUS TWIN-EYES OF SILICON & LIGHT    │
              │                                         │
              │   [ OLED: 128x64 ]    [ TFT: 128x160 ]   │
              │   Micro-Monochrome    16-Bit RGB565     │
              │   Sub-Pixel Oracle    Laser-Wipe Portal │
              └────────────────────┬────────────────────┘
                                   │
                                   v
             ┌───────────────────────────────────────────┐
             │       FREERTOS 240MHz DUAL-CORE ENGINE    │
             │   ANCS BLE Neural Link  •  Zambretti Storm │
             └───────────────────────────────────────────┘
```

Welcome to **CHAOSDESKY** — a dual-display cybernetic desk artifact forged at the crossroads of **ancient myth, philosophical entropy, and relentless microchip engineering**.

Where traditional hardware merely measures environment, **CHAOSDESKY** acts as a **Janus-faced Silicon Oracle**:
- **The OLED Eye** peers inward, observing the micro-vibrations of atmospheric pressure, molecular humidity, and quantum sub-pixel clock drift.
- **The Color TFT Eye** gazes outward into the digital void, parsing satellite weather streams from Hinjewadi Phase 1, warping through 3D cosmic starfields, and intercepting incoming cellular signals from your iPhone via BLE ANCS like a smartwatch bound in glass.

### 🏛️ The Mythos of the Machine
Like Prometheus stealing fire from Olympus, **CHAOSDESKY** steals raw electricity from 5V copper traces and ignites 240MHz of FreeRTOS silicon logic. It is named **CHAOS** because it thrives in entropy:
- It balances 11 dynamic color design system realities (Dracula, Cyberpunk, Nord, Matrix, Material You).
- It calculates atmospheric fate using the 100-year-old **Zambretti Barometric Algorithm**, predicting storms before the first drop falls.
- It blinks with the soul of a **Cute Pixel Cyber Cat**, winking through 128x64 monochrome pixels while floating hearts drift up into the void.

It is not just a desk clock. It is a **mindfuck of hardware and spirit** — a miniature monolith standing at the edge of your desk, reminding you that time is a construct, code is poetry, and order is born only when you tame the chaos.

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
