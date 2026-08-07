#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include "config.h"

struct SensorData {
    float tempC = 0.0f;
    float tempF = 0.0f;
    float humidity = 0.0f;
    float heatIndexC = 0.0f;
    float dewPointC = 0.0f;
    float pressureHpa = 1013.25f;
    float bmpTempC = 0.0f;
    float altitudeM = 0.0f;

    // Min/Max metrics
    float minTempC = 99.0f;
    float maxTempC = -99.0f;
    float minPressureHpa = 9999.0f;
    float maxPressureHpa = 0.0f;

    bool dhtValid = false;
    bool bmpValid = false;
};

class SensorManager {
public:
    SensorData data;
    float pressureHistory[SENSOR_HISTORY_SIZE];
    float tempHistory[SENSOR_HISTORY_SIZE];
    float humidityHistory[SENSOR_HISTORY_SIZE];
    int historyCount = 0;
    int historyIndex = 0;

    SensorManager();

    bool begin();
    void readSensors();
    void addHistorySample(float pressHpa, float tempC, float hum);
    float getPastPressure() const;
    float getPressureAt(int logicalIndex) const;
    float getTempAt(int logicalIndex) const;
    float getHumAt(int logicalIndex) const;

private:
    DHT dht;
    Adafruit_BMP085 bmp;
};

#endif // SENSORS_H
