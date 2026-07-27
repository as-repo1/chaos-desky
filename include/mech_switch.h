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
            static bool buttonState = HIGH;
            if (reading != buttonState) {
                buttonState = reading;

                // Button PRESSED (LOW because of INPUT_PULLUP to GND)
                if (buttonState == LOW) {
                    buttonPressTime = currentMs;
                    if (instantTrigger) {
                        detectedAction = SWITCH_SINGLE_CLICK;
                    }
                } 
                // Button RELEASED (HIGH)
                else {
                    unsigned long pressDuration = currentMs - buttonPressTime;
                    
                    if (pressDuration >= longPressTime) {
                        if (!instantTrigger) detectedAction = SWITCH_LONG_PRESS;
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
            if (buttonState == LOW && (currentMs - buttonPressTime >= longPressTime)) {
                detectedAction = SWITCH_LONG_PRESS;
                buttonPressTime = currentMs + 5000; // prevent re-triggering until released
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
    bool comboTriggered = false;
    const unsigned long comboThresholdMs = 80; // 80ms overlap threshold for instant combo trigger!

    bool update(MechSwitchManager& switch1, MechSwitchManager& switch2) {
        if (switch1.pin < 0 || switch2.pin < 0) return false;

        bool s1Active = (digitalRead(switch1.pin) == LOW);
        bool s2Active = (digitalRead(switch2.pin) == LOW);
        unsigned long currentMs = millis();

        if (s1Active && s2Active) {
            if (bothPressedStartTime == 0) {
                bothPressedStartTime = currentMs;
            } else if (!comboTriggered && (currentMs - bothPressedStartTime >= comboThresholdMs)) {
                comboTriggered = true;
                // Suppress both individual switch managers from firing single/double/long press on release!
                switch1.suppressNextAction = true;
                switch2.suppressNextAction = true;
                return true; // Simultaneous Combo Event Detected!
            }
        } else {
            if (!s1Active || !s2Active) {
                bothPressedStartTime = 0;
            }
            if (!s1Active && !s2Active) {
                comboTriggered = false;
            }
        }
        return false;
    }
};

#endif // MECH_SWITCH_H
