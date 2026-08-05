#include "sensors.h"

SensorManager::SensorManager() : dht(DHT_PIN, DHT_TYPE) {
    for (int i = 0; i < SENSOR_HISTORY_SIZE; i++) {
        pressureHistory[i] = 1013.25f;
        tempHistory[i] = 20.0f;
        humidityHistory[i] = 50.0f;
    }
}

bool SensorManager::begin() {
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

void SensorManager::readSensors() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h) && !isnan(t) && h > 0.0f) {
        data.humidity = h;
        data.tempC = t;
        data.tempF = (t * 9.0f / 5.0f) + 32.0f;
        data.heatIndexC = dht.computeHeatIndex(t, h, false);
        
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

    if (data.bmpValid) {
        float p = bmp.readPressure() / 100.0f;
        float bt = bmp.readTemperature();
        float alt = bmp.readAltitude(101325);

        if (p > 300.0f && p < 1200.0f) {
            data.pressureHpa = p;
            data.bmpTempC = bt;
            data.altitudeM = alt;

            if (data.pressureHpa < data.minPressureHpa) data.minPressureHpa = data.pressureHpa;
            if (data.pressureHpa > data.maxPressureHpa) data.maxPressureHpa = data.pressureHpa;
        }
    }
}

void SensorManager::addHistorySample(float pressHpa, float tempC, float hum) {
    if (historyCount < SENSOR_HISTORY_SIZE) {
        pressureHistory[historyCount] = pressHpa;
        tempHistory[historyCount] = tempC;
        humidityHistory[historyCount] = hum;
        historyCount++;
    } else {
        for (int i = 0; i < SENSOR_HISTORY_SIZE - 1; i++) {
            pressureHistory[i] = pressureHistory[i + 1];
            tempHistory[i] = tempHistory[i + 1];
            humidityHistory[i] = humidityHistory[i + 1];
        }
        pressureHistory[SENSOR_HISTORY_SIZE - 1] = pressHpa;
        tempHistory[SENSOR_HISTORY_SIZE - 1] = tempC;
        humidityHistory[SENSOR_HISTORY_SIZE - 1] = hum;
    }
}

float SensorManager::getPastPressure() const {
    if (historyCount == 0) return data.pressureHpa;
    return pressureHistory[0];
}
