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
    bool currentDebouncedState = HIGH; // Instance-specific debounced state
    
    int clickCount = 0;
    bool waitingForDoubleClick = false;
    bool suppressNextAction = false;
    bool instantTrigger = false; // If true, single click fires instantly on press
    const unsigned long debounceDelay = 35; // 35ms software debounce (no hardware capacitor needed!)
    const unsigned long doubleClickGap = 350; // 350ms window for double click
    const unsigned long longPressTime = 700; // 700ms for long press

    void begin(int gpioPin) {
        pin = gpioPin;
        if (pin >= 0) {
            pinMode(pin, INPUT_PULLUP); // 3.3V internal pull-up resistor (no external resistor needed!)
            lastState = digitalRead(pin);
            Serial.printf("⌨️ Mechanical Keyboard Switch Initialized on GPIO %d (INPUT_PULLUP)\n", pin);
        }
    }

    SwitchAction update() {
        if (pin < 0) return SWITCH_NO_ACTION;

        unsigned long currentMs = millis();
        bool reading = digitalRead(pin);

        // Software Debounce Filter
        if (reading != lastState) {
            lastDebounceTime = currentMs;
        }

        SwitchAction detectedAction = SWITCH_NO_ACTION;

        if ((currentMs - lastDebounceTime) > debounceDelay) {
            // State has settled after mechanical contact bounce
            if (reading != currentDebouncedState) {
                currentDebouncedState = reading;

                // Button PRESSED (LOW because of INPUT_PULLUP to GND)
                if (currentDebouncedState == LOW) {
                    buttonPressTime = currentMs;
                    if (instantTrigger) {
                        detectedAction = SWITCH_SINGLE_CLICK;
                    }
                } 
                // Button RELEASED (HIGH)
                else {
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

            // Check if double click window expired -> register single click
            if (waitingForDoubleClick && (currentMs - lastReleaseTime > doubleClickGap)) {
                waitingForDoubleClick = false;
                clickCount = 0;
                detectedAction = SWITCH_SINGLE_CLICK;
            }
            
            // Check for holding down (active long press before release)
            if (currentDebouncedState == LOW && (currentMs >= buttonPressTime) && (currentMs - buttonPressTime >= longPressTime)) {
                detectedAction = SWITCH_LONG_PRESS;
                buttonPressTime = currentMs + 86400000UL; // prevent re-triggering until released without integer overflow bug
                waitingForDoubleClick = false;
                clickCount = 0;
            }
        }

        lastState = reading;

        if (detectedAction != SWITCH_NO_ACTION && suppressNextAction) {
            detectedAction = SWITCH_NO_ACTION;
            suppressNextAction = false;
            waitingForDoubleClick = false;
            clickCount = 0;
        }

        return detectedAction;
    }
};

class DualSwitchComboDetector {
public:
    unsigned long bothPressedStartTime = 0;
    bool comboLongTriggered = false;
    const unsigned long comboThresholdMs = 80;     // 80ms overlap threshold for combo validity
    const unsigned long comboLongThresholdMs = 1500; // 1.5s for long combo (Hidden Page)

    int update(MechSwitchManager& switch1, MechSwitchManager& switch2) {
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
                    return 2; // Long combo triggered instantly
                }
            }
        } else {
            if (bothPressedStartTime != 0) {
                unsigned long duration = currentMs - bothPressedStartTime;
                bool wasLong = comboLongTriggered;
                
                bothPressedStartTime = 0;
                comboLongTriggered = false;
                
                if (!wasLong && duration >= comboThresholdMs && (s1Active || s2Active || (!s1Active && !s2Active))) {
                    // Only trigger short combo on release if it wasn't a long combo
                    switch1.suppressNextAction = true;
                    switch2.suppressNextAction = true;
                    return 1; // Short combo triggered on release
                }
            }
        }
        return 0;
    }
};

#endif // MECH_SWITCH_H
