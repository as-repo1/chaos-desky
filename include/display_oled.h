#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "config.h"
#include "sensors.h"
#include "notification_manager.h"
#include "screensaver.h"

extern ScreensaverEngine screensaverEngine;

class OledDisplayManager {
public:
    int oledMode = 0;
    uint8_t contrast = 255;
    bool isInverted = false;
    String customText = "Welcome to ChaosDesky!";
    int scrollX = 128;

    OledDisplayManager() : oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {}

    bool begin();
    void setContrast(uint8_t val);
    void setInverted(bool invert);
    void draw(SensorManager& sm, const NotificationManager& notifMgr, const String& ipStr, int h, int m, int s_val, int rssi, int clockStyle = 0);

private:
    Adafruit_SSD1306 oled;
    int lastH = -1, lastM = -1, lastS = -1;
    int lastRenderedMode = -1;
    int lastClockStyle = -1;
    float lastTemp = -999.0f;

    void drawNotificationOverlay(const NotificationItem& n, float progress);
    void drawMode6_Climate(const SensorData& s);
    void drawMode0_HUD(const SensorData& s, const String& ipStr, int h, int m, int s_val, int rssi);
    void drawMode1_BigClock(int h, int m, int s_val, const String& ipStr, const SensorData& s, int clockStyle = 0);
    void drawOledClock_DigitalHUD(int h, int m, int s_val, const String& ipStr, const SensorData& s);
    void drawOledClock_AnalogMinimal(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_CyberMatrix(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_RetroFlip(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_VerticalStack(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_BinaryGauges(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_CyberpunkBox(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_RadialHorizon(int h, int m, int s_val, const SensorData& s);
    void drawMode2_Sparklines(SensorManager& sm);
    void drawMode3_Marquee(const String& text);
    void drawMode5_WifiInfo(const String& ipStr, const String& ssid, const String& pass);
    void drawOledClock_Orbit(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_Word(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_DotMatrix(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_BarGraph(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_RadarSweep(int h, int m, int s_val, const SensorData& s);
    void drawOledClock_VintagePocket(int h, int m, int s_val, const SensorData& s);
};

#endif // DISPLAY_OLED_H
