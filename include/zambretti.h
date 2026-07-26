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
    static String calculateForecast(float currentPressureHpa, float pastPressureHpa, int month = 6) {
        // Calculate pressure change (hPa) over time interval
        float pDiff = currentPressureHpa - pastPressureHpa;
        PressureTrend trend = TREND_STEADY;

        if (pDiff > 1.5f) {
            trend = TREND_RISING;
        } else if (pDiff < -1.5f) {
            trend = TREND_FALLING;
        }

        // Zambretti formula Z score calculation
        int zScore = 0;
        if (trend == TREND_FALLING) {
            zScore = (int)(127.0f - (0.12f * currentPressureHpa));
            if (zScore < 1) zScore = 1;
            if (zScore > 9) zScore = 9;
        } else if (trend == TREND_RISING) {
            zScore = (int)(185.0f - (0.16f * currentPressureHpa));
            if (zScore < 10) zScore = 10;
            if (zScore > 21) zScore = 21;
        } else { // Steady
            zScore = (int)(144.0f - (0.13f * currentPressureHpa));
            if (zScore < 22) zScore = 22;
            if (zScore > 26) zScore = 26;
        }

        return getZambrettiText(zScore);
    }

private:
    static String getZambrettiText(int code) {
        switch (code) {
            case 1:  return "Settled Fine";
            case 2:  return "Fine Weather";
            case 3:  return "Fine, Becoming Less Settled";
            case 4:  return "Fairly Fine, Showers Later";
            case 5:  return "Showers Likely";
            case 6:  return "Unsettled, Rain Later";
            case 7:  return "Rain at Times";
            case 8:  return "Heavy Rain / Storm Warning";
            case 9:  return "Stormy, Severe Gales";
            case 10: return "Settled Fine";
            case 11: return "Fine, Becoming Warmer";
            case 12: return "Fairly Fine, Improving";
            case 13: return "Fairly Fine, Showers Early";
            case 14: return "Showery, Bright Intervals";
            case 15: return "Changeable, Improving";
            case 16: return "Unsettled, Clearing Later";
            case 17: return "Unsettled, Short Fine Intervals";
            case 18: return "Very Unsettled, Rain";
            case 19: return "Stormy, Clearing";
            case 20: return "Stormy, Much Rain";
            case 21: return "Settled Fine";
            case 22: return "Settled Fine";
            case 23: return "Fine, Possible Showers";
            case 24: return "Fairly Fine, Showers Likely";
            case 25: return "Unsettled, Short Fine Spells";
            case 26: return "Very Unsettled, Rain";
            default: return "Fine / Stable";
        }
    }
};

#endif // ZAMBRETTI_H
