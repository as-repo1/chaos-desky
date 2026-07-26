# chaos-desky — ESP32 Dual-Display Desk Companion

`chaos-desky` is an ESP32-based desktop companion driving two displays: a 1.8" color TFT (ST7735) and a 0.96" monochrome OLED (SSD1306). It displays climate data (DHT11, BMP180/280), OpenWeatherMap internet data for Hinjewadi 411057, iPhone BLE ANCS smartwatch notifications, customizable watch faces, animated screensavers, and an embedded LittleFS web control dashboard.

---

## Features

- **Dual Displays**:
  - **1.8" Color TFT (ST7735, 128x160)**: 10-page carousel (Weather, Barometer, Pomodoro, QR Code, Custom Image/Text, Indoor Climate, BLE Phone Notifs, Watch Face Studio, Network Monitor, 3D Warp Space).
  - **0.96" OLED (SSD1306, 128x64)**: 6 modes (Telemetry HUD, Digital Clock, Temp/Humidity Sparklines, Text Marquee, Uploaded Bitmap, Cyber Cat Mascot Screensaver).
- **Smartwatch BLE ANCS Notifications**: Pairs over Bluetooth as `ChaosDesky-Watch` to receive iPhone call, SMS, and app notifications with auto-dismiss popup banners.
- **Watch Face Studio**: 4 styles (Swiss Analog, Cyber Chrono, Modern Digital, Neon Nixie).
- **Embedded Web Portal**: 5 tabbed sections (Telemetry, Watch Faces, Smartwatch, Media, Settings) hosted via LittleFS.
- **Design System Themes**: 11 color themes (Cyberpunk, Matrix, Dark Glass, Retro, Dracula, Nord, Gruvbox, Monochrome, Nothing UI, One UI, Material You).
- **Smooth Animations**: Vertical laser scan wipe transitions between pages and 1-pixel scrolling marquee.
- **Default Display Orientation**: TFT rotated 180° by default.

---

## Hardware Pinouts

| Component | Pin / Bus | ESP32 GPIO |
| :--- | :--- | :--- |
| **OLED SSD1306** | I2C SDA / SCL | GPIO 21 / GPIO 22 |
| **BMP180 / BMP280** | I2C SDA / SCL | GPIO 21 / GPIO 22 |
| **DHT11** | Digital Data | GPIO 4 |
| **TFT ST7735** | SPI CS / DC / RST / MOSI / SCK | GPIO 15 / 16 / 17 / 13 / 14 |

---

## Circuit Mappings

```mermaid
graph TD
    ESP32[ESP32 Microcontroller]

    subgraph I2C_Bus ["I2C Bus (Shared)"]
        ESP32 -- "GPIO 21 (SDA)" --> OLED[0.96" OLED SSD1306]
        ESP32 -- "GPIO 22 (SCL)" --> OLED
        ESP32 -- "GPIO 21 (SDA)" --> BMP[BMP180/280 Sensor]
        ESP32 -- "GPIO 22 (SCL)" --> BMP
    end

    subgraph SPI_Bus ["SPI Bus (Dedicated)"]
        ESP32 -- "GPIO 13 (MOSI)" --> TFT[1.8" Color TFT ST7735]
        ESP32 -- "GPIO 14 (SCK)"  --> TFT
        ESP32 -- "GPIO 15 (CS)"   --> TFT
        ESP32 -- "GPIO 16 (DC)"   --> TFT
        ESP32 -- "GPIO 17 (RST)"  --> TFT
    end

    subgraph Digital_IO ["Digital IO"]
        ESP32 -- "GPIO 4 (DATA)"  --> DHT[DHT11 Sensor]
    end
```

---

## Build & Flash

1. Flash firmware and web dashboard filesystem:
   ```bash
   pio run -t upload && pio run -t uploadfs
   ```

2. Access the web portal:
   - Connect to `http://<ESP32_IP>` (e.g. `http://192.168.1.6`) or soft AP `ChaosDesky-AP`.
