#include "mech_switch.h"

void MechSwitchManager::begin(int gpioPin) {
    pin = gpioPin;
    if (pin >= 0) {
        pinMode(pin, INPUT_PULLUP);
        lastState = digitalRead(pin);
        Serial.printf("⌨️ Mechanical Keyboard Switch Initialized on GPIO %d (INPUT_PULLUP)\n", pin);
    }
}

SwitchAction MechSwitchManager::update() {
    if (pin < 0) return SWITCH_NO_ACTION;

    unsigned long currentMs = millis();
    bool reading = digitalRead(pin);

    if (reading != lastState) {
        lastDebounceTime = currentMs;
    }

    SwitchAction detectedAction = SWITCH_NO_ACTION;

    if ((currentMs - lastDebounceTime) > debounceDelay) {
        if (reading != currentDebouncedState) {
            currentDebouncedState = reading;

            if (currentDebouncedState == LOW) {
                buttonPressTime = currentMs;
                longPressTriggered = false;
                if (instantTrigger) {
                    detectedAction = SWITCH_SINGLE_CLICK;
                }
            } else {
                unsigned long pressDuration = (currentMs >= buttonPressTime) ? (currentMs - buttonPressTime) : 0;
                
                if (pressDuration >= longPressTime) {
                    detectedAction = SWITCH_LONG_PRESS;
                    waitingForDoubleClick = false;
                    clickCount = 0;
                } else if (pressDuration > 10 && !instantTrigger) {
                    clickCount++;
                    if (clickCount == 1) {
                        waitingForDoubleClick = true;
                        lastReleaseTime = currentMs;
                    } else if (clickCount == 2) {
                        detectedAction = SWITCH_DOUBLE_CLICK;
                        waitingForDoubleClick = false;
                        clickCount = 0;
                    }
                }
            }
        }

        if (waitingForDoubleClick && (currentMs - lastReleaseTime > doubleClickGap)) {
            waitingForDoubleClick = false;
            clickCount = 0;
            detectedAction = SWITCH_SINGLE_CLICK;
        }
        
        if (currentDebouncedState == LOW && (currentMs >= buttonPressTime) && (currentMs - buttonPressTime >= longPressTime)) {
            if (!longPressTriggered) {
                detectedAction = SWITCH_LONG_PRESS;
                longPressTriggered = true;
                waitingForDoubleClick = false;
                clickCount = 0;
            }
        }
    }

    lastState = reading;

    if (suppressNextAction) {
        if (detectedAction != SWITCH_NO_ACTION) {
            detectedAction = SWITCH_NO_ACTION;
            waitingForDoubleClick = false;
            clickCount = 0;
        }
        if (currentDebouncedState == HIGH && !waitingForDoubleClick) {
             suppressNextAction = false;
        }
    }

    return detectedAction;
}

int DualSwitchComboDetector::update(MechSwitchManager& switch1, MechSwitchManager& switch2) {
    if (switch1.pin < 0 || switch2.pin < 0) return 0;

    bool s1Active = (digitalRead(switch1.pin) == LOW);
    bool s2Active = (digitalRead(switch2.pin) == LOW);
    unsigned long currentMs = millis();

    if (s1Active && s2Active) {
        if (bothPressedStartTime == 0) {
            bothPressedStartTime = currentMs;
            comboLongTriggered = false;
        } else {
            unsigned long duration = currentMs - bothPressedStartTime;
            if (!comboLongTriggered && duration >= comboLongThresholdMs) {
                comboLongTriggered = true;
                switch1.suppressNextAction = true;
                switch2.suppressNextAction = true;
                return 2;
            }
        }
    } else {
        if (bothPressedStartTime != 0) {
            unsigned long duration = currentMs - bothPressedStartTime;
            bool wasLong = comboLongTriggered;
            
            bothPressedStartTime = 0;
            comboLongTriggered = false;
            
            if (!wasLong && duration >= comboThresholdMs && (s1Active || s2Active || (!s1Active && !s2Active))) {
                switch1.suppressNextAction = true;
                switch2.suppressNextAction = true;
                return 1;
            }
        }
    }
    return 0;
}
