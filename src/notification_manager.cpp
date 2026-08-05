#include "notification_manager.h"

void NotificationManager::trigger(const String& title, 
             const String& message, 
             NotificationCategory cat, 
             NotificationTarget target, 
             unsigned long durationSec) {
    
    NotificationTarget effectiveTarget = target;
    if (target == NOTIF_TARGET_USER_PREF) {
        effectiveTarget = userPreference;
    }
    currentNotif.title = title;
    currentNotif.message = message;
    currentNotif.category = cat;
    currentNotif.target = effectiveTarget;
    currentNotif.durationMs = durationSec * 1000;
    currentNotif.startMs = millis();
    currentNotif.active = true;

    Serial.printf("🔔 Notification Triggered! Target: %d (Pref: %d) | Title: %s | Msg: %s\n", 
                  (int)effectiveTarget, (int)userPreference, title.c_str(), message.c_str());
}

void NotificationManager::update() {
    if (currentNotif.active) {
        if (millis() - currentNotif.startMs >= currentNotif.durationMs) {
            currentNotif.active = false;
            Serial.println("🔔 Notification Expired & Dismissed.");
        }
    }
}

bool NotificationManager::isTftActive() const {
    return currentNotif.active && (currentNotif.target == NOTIF_TARGET_TFT || currentNotif.target == NOTIF_TARGET_BOTH);
}

bool NotificationManager::isOledActive() const {
    return currentNotif.active && (currentNotif.target == NOTIF_TARGET_OLED || currentNotif.target == NOTIF_TARGET_BOTH);
}

float NotificationManager::getProgress() const {
    if (!currentNotif.active || currentNotif.durationMs == 0) return 0.0f;
    unsigned long elapsed = millis() - currentNotif.startMs;
    if (elapsed >= currentNotif.durationMs) return 1.0f;
    return (float)elapsed / (float)currentNotif.durationMs;
}
