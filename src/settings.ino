/*

ESP69GW
SETTINGS MODULE

ESP8266 to RFM69 Gateway

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "Embedis.h"
#include <EEPROM.h>
#include "spi_flash.h"

#define AUTO_SAVE 0
Embedis embedis(Serial);

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

void settingsSetup() {
    EEPROM.begin(SPI_FLASH_SEC_SIZE);
    Embedis::dictionary( F("EEPROM"),
        SPI_FLASH_SEC_SIZE,
        [](size_t pos) -> char { return EEPROM.read(pos); },
        [](size_t pos, char value) { EEPROM.write(pos, value); },
        #if AUTO_SAVE
            []() { EEPROM.commit(); }
        #else
            []() {}
        #endif
    );
    #if DEBUG
        Serial.println("[SETTINGS] Initialized");
    #endif
}

void settingsLoop() {
    embedis.process();
}

String getSetting(const String& key, String defaultValue) {
    String value;
    if (!Embedis::get(key, value)) value = defaultValue;
    return value;
}

bool setSetting(const String& key, String& value) {
    return Embedis::set(key, value);
}

bool delSetting(const String& key) {
    return Embedis::del(key);
}

void saveSettings() {
    #if DEBUG
        Serial.println("[SETTINGS] Saving");
    #endif
    #if not AUTO_SAVE
        EEPROM.commit();
    #endif
}
