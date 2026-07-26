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
    float pressureHistory[PRESSURE_HISTORY_SIZE];
    int historyCount = 0;

    SensorManager() {
        for (int i = 0; i < PRESSURE_HISTORY_SIZE; i++) {
            pressureHistory[i] = 1013.25f;
        }
    }

    bool begin() {
        Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
        dht.begin();
        
        data.bmpValid = bmp.begin(BMP085_ULTRAHIGHRES, &Wire);
        if (!data.bmpValid) {
            Serial.println("⚠️ BMP180/280 Sensor not found on I2C!");
        } else {
            Serial.println("✅ BMP180/280 Sensor initialized!");
        }

        readSensors();
        return true;
    }

    void readSensors() {
        // Read DHT11
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        if (!isnan(h) && !isnan(t) && h > 0.0f) {
            data.humidity = h;
            data.tempC = t;
            data.tempF = (t * 9.0f / 5.0f) + 32.0f;
            data.heatIndexC = dht.computeHeatIndex(t, h, false);
            
            // Magnus formula for Dew Point
            float a = 17.27f;
            float b = 237.7f;
            float alpha = ((a * t) / (b + t)) + log(h / 100.0f);
            data.dewPointC = (b * alpha) / (a - alpha);

            data.dhtValid = true;

            if (data.tempC < data.minTempC) data.minTempC = data.tempC;
            if (data.tempC > data.maxTempC) data.maxTempC = data.tempC;
        } else {
            data.dhtValid = false;
        }

        // Read BMP180 / BMP280
        if (data.bmpValid) {
            float p = bmp.readPressure() / 100.0f; // Convert Pa to hPa
            float bt = bmp.readTemperature();
            float alt = bmp.readAltitude(101325);  // Meters

            if (p > 300.0f && p < 1200.0f) {
                data.pressureHpa = p;
                data.bmpTempC = bt;
                data.altitudeM = alt;

                if (data.pressureHpa < data.minPressureHpa) data.minPressureHpa = data.pressureHpa;
                if (data.pressureHpa > data.maxPressureHpa) data.maxPressureHpa = data.pressureHpa;
            }
        }
    }

    void addPressureSample(float pressHpa) {
        if (historyCount < PRESSURE_HISTORY_SIZE) {
            pressureHistory[historyCount++] = pressHpa;
        } else {
            // Shift array left
            for (int i = 0; i < PRESSURE_HISTORY_SIZE - 1; i++) {
                pressureHistory[i] = pressureHistory[i + 1];
            }
            pressureHistory[PRESSURE_HISTORY_SIZE - 1] = pressHpa;
        }
    }

    float getPastPressure() {
        if (historyCount == 0) return data.pressureHpa;
        return pressureHistory[0];
    }

private:
    DHT dht{DHT_PIN, DHT_TYPE};
    Adafruit_BMP085 bmp;
};

#endif // SENSORS_H
