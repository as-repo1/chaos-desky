#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// 🔌 HARDWARE PIN DEFINITIONS
// ==========================================

// OLED SSD1306 Display (I2C)
#define OLED_SDA_PIN    21
#define OLED_SCL_PIN    22
#define OLED_I2C_ADDR   0x3C
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64

// BMP180 / BMP280 Sensor (I2C)
#define BMP_SDA_PIN     21
#define BMP_SCL_PIN     22

// DHT11 Sensor (Digital IO)
#define DHT_PIN         4
#define DHT_TYPE        DHT11

// TFT ST7735 Display (SPI)
#define TFT_CS_PIN      15
#define TFT_DC_PIN      16
#define TFT_RST_PIN     17
#define TFT_MOSI_PIN    13
#define TFT_SCLK_PIN    14
#define TFT_WIDTH       128
#define TFT_HEIGHT      160
#define TFT_INIT_TAB    INITR_BLACKTAB // Options: INITR_BLACKTAB, INITR_REDTAB, INITR_GREENTAB
#define TFT_ROTATION    2              // 0 = 0deg, 1 = 90deg, 2 = 180deg, 3 = 270deg

// Optional Pins
#define BUZZER_PIN      -1    // Set to GPIO pin if piezo buzzer attached (-1 = disabled)
#define MECH_SWITCH_1_PIN 25    // ⌨️ Switch 1: Navigation & Slideshow Toggle (Pin to D25 & GND)
#define MECH_SWITCH_2_PIN 26    // 🎮 Switch 2: TFT Interactive Action Switch (Pin to D26 & GND)
#define MECH_SWITCH_PIN   MECH_SWITCH_1_PIN // Backward compatible alias

// ==========================================
// 🌐 WIFI & NETWORK CONFIGURATION
// ==========================================
#define DEFAULT_WIFI_SSID     "Airtel_a204"
#define DEFAULT_WIFI_PASS     "rahulkhanki"
#define AP_SSID               "ChaosDesky-AP"
#define AP_PASS               "12345678"
#define NTP_SERVER_1          "pool.ntp.org"
#define NTP_SERVER_2          "time.nist.gov"
#define GMT_OFFSET_SEC        19800 // GMT+5:30 (adjust to your timezone)
#define DAYLIGHT_OFFSET_SEC   0

// ==========================================
// ☀️ OPENWEATHER API CONFIGURATION
// ==========================================
#define OPENWEATHER_API_KEY   "YOUR_OPENWEATHER_API_KEY"
#define OPENWEATHER_CITY      "London"
#define OPENWEATHER_COUNTRY   "UK"
#define OPENWEATHER_UNITS     "metric" // "metric" (°C) or "imperial" (°F)
#define WEATHER_UPDATE_MS     (10 * 60 * 1000) // Fetch every 10 minutes

// ==========================================
// 📊 UI & CAROUSEL CONFIGURATION
// ==========================================
#define CAROUSEL_INTERVAL_MS  0     // Default to 0 (Manual Navigation Only)
#define TOTAL_TFT_PAGES       13    // 13 Total TFT Carousel Pages (0-12)
#define NUM_OLED_MODES        6     // 6 OLED Display Modes
#define TOTAL_THEMES          11    // 11 Themes

// Pressure Trend History Buffer
#define PRESSURE_HISTORY_SIZE 24   // 24 samples for sparkline graph
#define PRESSURE_SAMPLE_MS    (15 * 60 * 1000) // Sample pressure every 15 mins

// Pomodoro Timer Defaults
#define POMODORO_WORK_MINS    25
#define POMODORO_BREAK_MINS   5

#endif // CONFIG_H
