#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <Arduino.h>

enum NotificationCategory {
    NOTIF_INFO = 0,
    NOTIF_MESSAGE = 1,
    NOTIF_CALL = 2,
    NOTIF_WARNING = 3,
    NOTIF_ALERT = 4
};

enum NotificationTarget {
    NOTIF_TARGET_TFT = 0,
    NOTIF_TARGET_OLED = 1,
    NOTIF_TARGET_BOTH = 2
};

struct NotificationItem {
    String title = "";
    String message = "";
    NotificationCategory category = NOTIF_INFO;
    NotificationTarget target = NOTIF_TARGET_BOTH;
    unsigned long durationMs = 8000;
    unsigned long startMs = 0;
    bool active = false;
};

class NotificationManager {
public:
    NotificationItem currentNotif;

    void trigger(const String& title, 
                 const String& message, 
                 NotificationCategory cat = NOTIF_INFO, 
                 NotificationTarget target = NOTIF_TARGET_BOTH, 
                 unsigned long durationSec = 8) {
        
        currentNotif.title = title;
        currentNotif.message = message;
        currentNotif.category = cat;
        currentNotif.target = target;
        currentNotif.durationMs = durationSec * 1000;
        currentNotif.startMs = millis();
        currentNotif.active = true;

        Serial.printf("🔔 Notification Triggered! Target: %d | Title: %s | Msg: %s\n", 
                      (int)target, title.c_str(), message.c_str());
    }

    void update() {
        if (currentNotif.active) {
            if (millis() - currentNotif.startMs >= currentNotif.durationMs) {
                currentNotif.active = false;
                Serial.println("🔔 Notification Expired & Dismissed.");
            }
        }
    }

    bool isTftActive() const {
        return currentNotif.active && (currentNotif.target == NOTIF_TARGET_TFT || currentNotif.target == NOTIF_TARGET_BOTH);
    }

    bool isOledActive() const {
        return currentNotif.active && (currentNotif.target == NOTIF_TARGET_OLED || currentNotif.target == NOTIF_TARGET_BOTH);
    }

    float getProgress() const {
        if (!currentNotif.active || currentNotif.durationMs == 0) return 0.0f;
        unsigned long elapsed = millis() - currentNotif.startMs;
        if (elapsed >= currentNotif.durationMs) return 1.0f;
        return (float)elapsed / (float)currentNotif.durationMs;
    }
};

#endif // NOTIFICATION_MANAGER_H
