# 📜 Changelog — `chaos-desky`

All notable changes to the **chaos-desky** project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-07-27

### ✨ Added
- **Dual Display System**:
  - Monochrome OLED (SSD1306 128x64) real-time sensor HUD displaying local temperature, humidity, pressure, altitude, heat index, dew point, WiFi RSSI, and NTP clock.
  - Color TFT (ST7735 128x160) 4-page dynamic carousel engine (10-second auto-switch or web remote control).
- **Page 1 (Outdoor Internet Weather)**:
  - Integration with OpenWeatherMap API fetching live outdoor temperature, min/max forecast, humidity, wind speed, and weather condition graphics.
- **Page 2 (Pressure Graph & Zambretti Predictor)**:
  - 24-sample barometric pressure sparkline trend graph.
  - Zambretti storm prediction algorithm determining atmospheric trends ($\Delta P / \Delta t$) and printing local predictions (*"Settled Fine"*, *"Showers Likely"*, *"Storm Warning"*).
  - Comfort Index meter (Ideal / Humid / Dry).
- **Page 3 (Cyberpunk Pomodoro Hub)**:
  - Digital clock synced via NTP.
  - 25m Work / 5m Rest Pomodoro focus timer with visual progress bar and completed session counter.
- **Page 4 (System Telemetry & Dynamic QR Code)**:
  - ESP32 RAM free heap meter, CPU clock frequency, uptime.
  - On-the-fly generated **QR Code** using `ricmoo/QRCode` pointing directly to `http://<ESP32_IP>`.
- **Embedded Web Dashboard (LittleFS + AsyncWebServer)**:
  - Dark glassmorphism responsive web UI.
  - REST API endpoints for sensor metrics, weather, pomodoro timer control, page switcher, and color theme engine (*Cyberpunk*, *Matrix*, *Dark Glass*, *Retro Arcade*).
- **Firmware & Build Configuration**:
  - FreeRTOS dual-core task division (Core 0 for Network & Web Server, Core 1 for Sensor sampling & Display rendering).
  - PlatformIO configuration (`platformio.ini`) supporting `esp32dev` with LittleFS filesystem integration.
- **Comprehensive Documentation**:
  - `README.md`, `README_ARCHITECTURE.md`, `README_PIN_DIAGRAM.md`, and `CHANGELOG.md`.
