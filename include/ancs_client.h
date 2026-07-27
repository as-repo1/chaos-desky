#ifndef ANCS_CLIENT_H
#define ANCS_CLIENT_H

#include <Arduino.h>
#include "notification_manager.h"

extern NotificationManager notificationMgr;

struct PhoneNotificationLog {
    String lastApp = "None";
    String lastSender = "No Notifications";
    String lastMessage = "Waiting for Web/Push events...";
    unsigned long lastTimeMs = 0;
    int totalCount = 0;
    bool connected = true;
};

class AncsNotificationClient {
public:
    PhoneNotificationLog phoneLog;

    bool begin() {
        Serial.println("✅ Push Notification Receiver Ready (BLE Disabled)");
        return true;
    }

    void simulateNotification(const String& sender, const String& text, NotificationCategory cat = NOTIF_MESSAGE) {
        phoneLog.lastApp = (cat == NOTIF_CALL) ? "Phone Call" : "WhatsApp";
        phoneLog.lastSender = sender;
        phoneLog.lastMessage = text;
        phoneLog.lastTimeMs = millis();
        phoneLog.totalCount++;

        notificationMgr.trigger(sender, text, cat, NOTIF_TARGET_USER_PREF, 10);
    }
};

#endif // ANCS_CLIENT_H
