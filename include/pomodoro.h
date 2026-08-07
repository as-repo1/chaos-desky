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
    PomodoroState pausedFrom = POMO_WORK;
    unsigned long durationSec = POMODORO_WORK_MINS * 60;
    unsigned long remainingSec = POMODORO_WORK_MINS * 60;
    unsigned long lastTickMs = 0;
    int completedSessions = 0;
    int workDurationMins = POMODORO_WORK_MINS;
    int breakDurationMins = POMODORO_BREAK_MINS;

    void startWork();
    void startBreak();
    void pause();
    void resume();
    void reset();
    void update();
    float getProgress() const;
    const char* getFormattedTime() const;
    const char* getStateString() const;
};

#endif // POMODORO_H
