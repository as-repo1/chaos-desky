#ifndef MECH_SWITCH_H
#define MECH_SWITCH_H

#include <Arduino.h>

enum SwitchAction {
    SWITCH_NO_ACTION = 0,
    SWITCH_SINGLE_CLICK = 1,
    SWITCH_DOUBLE_CLICK = 2,
    SWITCH_LONG_PRESS = 3
};

class MechSwitchManager {
public:
    int pin = -1;
    bool lastState = HIGH;
    unsigned long lastDebounceTime = 0;
    unsigned long buttonPressTime = 0;
    unsigned long lastReleaseTime = 0;
    bool currentDebouncedState = HIGH;
    bool longPressTriggered = false;    
    int clickCount = 0;
    bool waitingForDoubleClick = false;
    bool suppressNextAction = false;
    bool instantTrigger = false;
    const unsigned long debounceDelay = 35;
    const unsigned long doubleClickGap = 350;
    const unsigned long longPressTime = 700;

    void begin(int gpioPin);
    SwitchAction update();
};

class DualSwitchComboDetector {
public:
    unsigned long bothPressedStartTime = 0;
    bool comboLongTriggered = false;
    const unsigned long comboThresholdMs = 80;
    const unsigned long comboLongThresholdMs = 1500;

    int update(MechSwitchManager& switch1, MechSwitchManager& switch2);
};

#endif // MECH_SWITCH_H
