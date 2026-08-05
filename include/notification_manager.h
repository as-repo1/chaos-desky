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
    NOTIF_TARGET_BOTH = 2,
    NOTIF_TARGET_USER_PREF = 3
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
    NotificationTarget userPreference = NOTIF_TARGET_BOTH;

    void trigger(const String& title, 
                 const String& message, 
                 NotificationCategory cat = NOTIF_INFO, 
                 NotificationTarget target = NOTIF_TARGET_USER_PREF, 
                 unsigned long durationSec = 8);

    void update();
    bool isTftActive() const;
    bool isOledActive() const;
    float getProgress() const;
};

#endif // NOTIFICATION_MANAGER_H
