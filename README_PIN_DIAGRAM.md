# 🔌 Hardware Wiring & Pin Diagram — `chaos-desky`

This guide outlines the pin connections, hardware protocols, power requirements, and wiring diagrams for the **`chaos-desky`** dual-display station.

---

## 📌 Complete Pin Mapping Table

| Component | Module Pin | ESP32 GPIO | Protocol | Power | Notes |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **OLED (SSD1306)** | SDA | `GPIO 21` | I2C | `3.3V` | Shared I2C bus (`0x3C`) |
| | SCL | `GPIO 22` | I2C | `3.3V` | Shared I2C bus |
| | VCC / GND | `3.3V` / `GND` | Power | — | Monochrome 128x64 display |
| **BMP180 / BMP280** | SDA | `GPIO 21` | I2C | `3.3V` | Shared I2C bus (`0x77` / `0x76`) |
| | SCL | `GPIO 22` | I2C | `3.3V` | Shared I2C bus |
| | VCC / GND | `3.3V` / `GND` | Power | — | Barometric pressure & altitude |
| **DHT11 Sensor** | DATA | `GPIO 4` | Digital IO | `3.3V` | Requires 10k ohm pull-up resistor to 3.3V |
| | VCC / GND | `3.3V` / `GND` | Power | — | Temperature & Humidity |
| **TFT Display (ST7735)** | MOSI (SDA) | `GPIO 13` | SPI | `3.3V` | Hardware SPI Data |
| | SCK (SCL) | `GPIO 14` | SPI | `3.3V` | Hardware SPI Clock |
| | CS | `GPIO 15` | SPI | `3.3V` | Chip Select |
| | DC (A0) | `GPIO 16` | Control | `3.3V` | Data / Command |
| | RST (RES) | `GPIO 17` | Control | `3.3V` | Reset pin |
| | BL (LED) | `3.3V` | Power | `3.3V` | Backlight (or connect to GPIO for PWM) |
| | VCC / GND | `3.3V` / `GND` | Power | — | Color 128x160 TFT display |

---

## 📐 Circuit Schematic Diagram

```mermaid
graph LR
    ESP[ESP32 Microcontroller]

    subgraph I2C_Bus ["I2C Bus (Shared)"]
        ESP -- "GPIO 21 (SDA)" --> OLED[0.96" OLED SSD1306]
        ESP -- "GPIO 22 (SCL)" --> OLED
        ESP -- "GPIO 21 (SDA)" --> BMP[BMP180/BMP280 Sensor]
        ESP -- "GPIO 22 (SCL)" --> BMP
    end

    subgraph SPI_Bus ["SPI Bus (Dedicated)"]
        ESP -- "GPIO 13 (MOSI)" --> TFT[1.8" Color TFT ST7735]
        ESP -- "GPIO 14 (SCK)"  --> TFT
        ESP -- "GPIO 15 (CS)"   --> TFT
        ESP -- "GPIO 16 (DC)"   --> TFT
        ESP -- "GPIO 17 (RST)"  --> TFT
    end

    subgraph Digital_IO ["Digital IO"]
        ESP -- "GPIO 4 (DATA)"  --> DHT[DHT11 Sensor]
    end

    subgraph Power_Rails ["Power Rails"]
        3V3[3.3V Rail] --> OLED
        3V3 --> TFT
        3V3 --> DHT
        3V3 --> BMP
        GND[Ground]    --> OLED
        GND --> TFT
        GND --> DHT
        GND --> BMP
    end
```

---

## 🛠️ Assembly & Build Advice

1. **Power Budget**:
   - The ESP32's onboard 3.3V linear regulator can easily supply the OLED, BMP sensor, DHT11, and TFT display backlight under standard USB 5V power (~120mA total current draw).
   - If the TFT display flickers during WiFi activity, ensure you are using a good quality USB power cable.

2. **I2C Bus Sharing**:
   - Both the **OLED (SSD1306)** and **BMP180/BMP280** share `GPIO 21 (SDA)` and `GPIO 22 (SCL)`.
   - Ensure neither device has conflicting I2C addresses (OLED is default `0x3C`, BMP is default `0x77` or `0x76`).

3. **DHT11 Pull-up Resistor**:
   - If your DHT11 is a bare 4-pin blue module (not mounted on a PCB breakout), place a `10k Ohm` resistor between the `DATA (GPIO 4)` pin and `3.3V`.

4. **TFT Screen Orientation**:
   - The TFT display rotation is set to 180° by default (`TFT_ROTATION 2` in `config.h`).
