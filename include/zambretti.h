#ifndef ZAMBRETTI_H
#define ZAMBRETTI_H

#include <Arduino.h>

enum PressureTrend {
    TREND_FALLING = 0,
    TREND_STEADY  = 1,
    TREND_RISING  = 2
};

class ZambrettiForecaster {
public:
    static String calculateForecast(float currentPressureHpa, float pastPressureHpa, int month = 6);

private:
    static String getZambrettiText(int code);
};

#endif // ZAMBRETTI_H
