#include "pomodoro.h"

void PomodoroTimer::startWork() {
    state = POMO_WORK;
    durationSec = workDurationMins * 60;
    remainingSec = durationSec;
    lastTickMs = millis();
}

void PomodoroTimer::startBreak() {
    state = POMO_BREAK;
    durationSec = breakDurationMins * 60;
    remainingSec = durationSec;
    lastTickMs = millis();
}

void PomodoroTimer::pause() {
    if (state == POMO_WORK || state == POMO_BREAK) {
        pausedFrom = state;
        state = POMO_PAUSED;
    }
}

void PomodoroTimer::resume() {
    if (state == POMO_PAUSED) {
        state = pausedFrom;
        lastTickMs = millis();
    }
}

void PomodoroTimer::reset() {
    state = POMO_IDLE;
    durationSec = workDurationMins * 60;
    remainingSec = durationSec;
}

void PomodoroTimer::update() {
    if (state != POMO_WORK && state != POMO_BREAK) return;

    unsigned long now = millis();
    if (now - lastTickMs >= 1000) {
        unsigned long elapsedSec = (now - lastTickMs) / 1000;
        lastTickMs = now;

        if (remainingSec > elapsedSec) {
            remainingSec -= elapsedSec;
        } else {
            remainingSec = 0;
            if (state == POMO_WORK) {
                completedSessions++;
                startBreak();
            } else if (state == POMO_BREAK) {
                reset();
            }
        }
    }
}

float PomodoroTimer::getProgress() const {
    if (durationSec == 0) return 0.0f;
    return (float)(durationSec - remainingSec) / (float)durationSec;
}

String PomodoroTimer::getFormattedTime() const {
    unsigned long mins = remainingSec / 60;
    unsigned long secs = remainingSec % 60;
    char buf[10];
    snprintf(buf, sizeof(buf), "%02lu:%02lu", mins, secs);
    return String(buf);
}

String PomodoroTimer::getStateString() const {
    switch (state) {
        case POMO_WORK:   return "WORK";
        case POMO_BREAK:  return "REST";
        case POMO_PAUSED: return "PAUSED";
        default:          return "READY";
    }
}
