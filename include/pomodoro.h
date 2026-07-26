#ifndef POMODORO_H
#define POMODORO_H

#include <Arduino.h>
#include "config.h"

enum PomodoroState {
    POMO_IDLE = 0,
    POMO_WORK,
    POMO_BREAK,
    POMO_PAUSED
};

class PomodoroTimer {
public:
    PomodoroState state = POMO_IDLE;
    unsigned long durationSec = POMODORO_WORK_MINS * 60;
    unsigned long remainingSec = POMODORO_WORK_MINS * 60;
    unsigned long lastTickMs = 0;
    int completedSessions = 0;
    int workDurationMins = POMODORO_WORK_MINS;
    int breakDurationMins = POMODORO_BREAK_MINS;

    void startWork() {
        state = POMO_WORK;
        durationSec = workDurationMins * 60;
        remainingSec = durationSec;
        lastTickMs = millis();
    }

    void startBreak() {
        state = POMO_BREAK;
        durationSec = breakDurationMins * 60;
        remainingSec = durationSec;
        lastTickMs = millis();
    }

    void pause() {
        if (state == POMO_WORK || state == POMO_BREAK) {
            state = POMO_PAUSED;
        }
    }

    void resume() {
        if (state == POMO_PAUSED) {
            state = (remainingSec > breakDurationMins * 60) ? POMO_WORK : POMO_BREAK;
            lastTickMs = millis();
        }
    }

    void reset() {
        state = POMO_IDLE;
        durationSec = workDurationMins * 60;
        remainingSec = durationSec;
    }

    void update() {
        if (state != POMO_WORK && state != POMO_BREAK) return;

        unsigned long now = millis();
        if (now - lastTickMs >= 1000) {
            unsigned long elapsedSec = (now - lastTickMs) / 1000;
            lastTickMs = now;

            if (remainingSec > elapsedSec) {
                remainingSec -= elapsedSec;
            } else {
                remainingSec = 0;
                // Transition state
                if (state == POMO_WORK) {
                    completedSessions++;
                    startBreak();
                } else if (state == POMO_BREAK) {
                    reset();
                }
            }
        }
    }

    float getProgress() {
        if (durationSec == 0) return 0.0f;
        return (float)(durationSec - remainingSec) / (float)durationSec;
    }

    String getFormattedTime() {
        unsigned long mins = remainingSec / 60;
        unsigned long secs = remainingSec % 60;
        char buf[10];
        snprintf(buf, sizeof(buf), "%02lu:%02lu", mins, secs);
        return String(buf);
    }

    String getStateString() {
        switch (state) {
            case POMO_WORK:   return "WORK";
            case POMO_BREAK:  return "REST";
            case POMO_PAUSED: return "PAUSED";
            default:          return "READY";
        }
    }
};

#endif // POMODORO_H
