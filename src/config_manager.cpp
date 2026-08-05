#include "config_manager.h"

bool ConfigManager::begin() {
    if (!LittleFS.exists("/config.json")) {
        Serial.println("⚙️ No /config.json found. Saving default configuration...");
        return saveConfig();
    }
    return loadConfig();
}

bool ConfigManager::loadConfig() {
    File file = LittleFS.open("/config.json", "r");
    if (!file) {
        Serial.println("❌ Failed to open /config.json for reading!");
        return false;
    }

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("❌ Failed to parse /config.json: " + String(error.c_str()));
        return false;
    }

    String s = doc["wifiSsid"] | DEFAULT_WIFI_SSID;
    String p = doc["wifiPass"] | DEFAULT_WIFI_PASS;

    if (s.isEmpty() || s == "YOUR_WIFI_SSID") {
        s = DEFAULT_WIFI_SSID;
        p = DEFAULT_WIFI_PASS;
    }

    config.wifiSsid        = s;
    config.wifiPass        = p;
    config.openWeatherKey  = doc["openWeatherKey"] | OPENWEATHER_API_KEY;
    config.openWeatherCity = doc["openWeatherCity"] | OPENWEATHER_CITY;
    config.openWeatherCountry = doc["openWeatherCountry"] | OPENWEATHER_COUNTRY;

    config.tftTheme        = doc["tftTheme"] | 0;
    config.tftRotation     = doc["tftRotation"] | 2;
    config.carouselSpeedSec= doc["carouselSpeedSec"] | 0;
    config.enabledPagesMask= doc["enabledPagesMask"] | 0x03FF;
    if ((config.enabledPagesMask & 0x03FF) <= 0x01FF) {
        config.enabledPagesMask |= (1 << 9); // Auto-enable OLED Studio Page on migration
    }
    if (doc.containsKey("tT")) {
        for (int i = 0; i < 4; i++) {
            config.todoTitles[i] = doc["tT"][i].as<String>();
            config.todoChecked[i] = doc["tC"][i] | false;
        }
    }

    config.oledMode        = doc["oledMode"] | 0;
    config.oledClockStyle  = doc["oledClockStyle"] | 0;
    config.oledContrast    = doc["oledContrast"] | 255;
    config.oledInverted    = doc["oledInverted"] | false;
    config.customText      = doc["customText"] | "Welcome to ChaosDesky!";

    config.pomoWorkMins    = doc["pomoWorkMins"] | POMODORO_WORK_MINS;
    config.pomoBreakMins   = doc["pomoBreakMins"] | POMODORO_BREAK_MINS;

    config.featureBleEnabled         = doc["featureBleEnabled"] | true;
    config.featureWeatherEnabled     = doc["featureWeatherEnabled"] | true;
    config.featureScreensaverEnabled = doc["featureScreensaverEnabled"] | true;
    config.notifTarget               = doc["notifTarget"] | 1;
    config.btnCombo                  = doc["btnCombo"] | 7;

    config.hidMacroLeft              = doc["hidMacroLeft"] | DEFAULT_MACRO_LEFT;
    config.hidMacroRight             = doc["hidMacroRight"] | DEFAULT_MACRO_RIGHT;

    Serial.println("✅ Configuration loaded successfully from LittleFS!");
    return true;
}

bool ConfigManager::saveConfig() {
    File file = LittleFS.open("/config.json", "w");
    if (!file) {
        Serial.println("❌ Failed to open /config.json for writing!");
        return false;
    }

    StaticJsonDocument<1024> doc;
    doc["wifiSsid"]          = config.wifiSsid;
    doc["wifiPass"]          = config.wifiPass;
    doc["openWeatherKey"]    = config.openWeatherKey;
    doc["openWeatherCity"]   = config.openWeatherCity;
    doc["openWeatherCountry"]= config.openWeatherCountry;

    doc["tftTheme"]          = config.tftTheme;
    doc["tftRotation"]       = config.tftRotation;
    doc["carouselSpeedSec"]  = config.carouselSpeedSec;
    doc["enabledPagesMask"]  = config.enabledPagesMask;

    JsonArray tT = doc.createNestedArray("tT");
    JsonArray tC = doc.createNestedArray("tC");
    for (int i = 0; i < 4; i++) {
        tT.add(config.todoTitles[i]);
        tC.add(config.todoChecked[i]);
    }

    doc["oledMode"]          = config.oledMode;
    doc["oledClockStyle"]    = config.oledClockStyle;
    doc["oledContrast"]      = config.oledContrast;
    doc["oledInverted"]      = config.oledInverted;
    doc["customText"]        = config.customText;

    doc["pomoWorkMins"]      = config.pomoWorkMins;
    doc["pomoBreakMins"]     = config.pomoBreakMins;

    doc["featureBleEnabled"]         = config.featureBleEnabled;
    doc["featureWeatherEnabled"]     = config.featureWeatherEnabled;
    doc["featureScreensaverEnabled"] = config.featureScreensaverEnabled;
    doc["notifTarget"]               = config.notifTarget;
    doc["btnCombo"]                  = config.btnCombo;

    doc["hidMacroLeft"]              = config.hidMacroLeft;
    doc["hidMacroRight"]             = config.hidMacroRight;

    if (serializeJson(doc, file) == 0) {
        Serial.println("❌ Failed to write JSON to /config.json!");
        file.close();
        return false;
    }

    file.close();
    Serial.println("✅ Saved configuration to LittleFS /config.json!");
    return true;
}
