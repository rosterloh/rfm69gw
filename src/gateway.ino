/*

ESP69GW
MAIN MODULE

ESP8266 to RFM69 Gateway
Gateway code with suport for ESP8266-based boards

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "defaults.h"
#include "RFM69Manager.h"

String getSetting(const String& key, String defaultValue = "");
uint8_t packetIDs[255] = {0};

// -----------------------------------------------------------------------------
// Common methods
// -----------------------------------------------------------------------------

void processMessage(packet_t * data) {

    blink(3);

    #if DEBUG
        Serial.print("[MESSAGE]");
        Serial.print(" messageID:");
        Serial.print(data->messageID);
        Serial.print(" nodeID:");
        Serial.print(data->nodeID);
        Serial.print(" packetID:");
        Serial.print(data->packetID);
        Serial.print(" name:");
        Serial.print(data->name);
        Serial.print(" value: ");
        Serial.print(data->value);
        Serial.print(" rssi:");
        Serial.print(data->rssi);
    #endif

    // Detect duplicates and missing packets
    // packetID==0 means device is not sending packetID info
    if (data->packetID > 0) {

        if (packetIDs[data->nodeID] == data->packetID) {
            #if DEBUG
                Serial.println(" DUPLICATED");
            #endif
            return;
        }

        #if DEBUG
            if (packetIDs[data->nodeID] > 0) {
                if (packetIDs[data->nodeID] != (data->packetID-1)) {
                    Serial.println(" MISSING PACKETS!!");
                }
            }
        #endif

    }
    packetIDs[data->nodeID] = data->packetID;

    #if DEBUG
        Serial.println();
    #endif

    // Try to find a matching mapping
    bool found = false;
    unsigned int count = getSetting("mappingCount", "0").toInt();
    for (unsigned int i=0; i<count; i++) {
        if ((getSetting("nodeid" + String(i)) == String(data->nodeID)) &&
            (getSetting("key" + String(i)) == data->name)) {
            mqttSend((char *) getSetting("topic" + String(i)).c_str(), (char *) String(data->value).c_str());
            found = true;
            break;
        }
    }

    if (!found) {
        String topic = getSetting("defaultTopic");
        if (topic.length() > 0) {
            topic.replace("{nodeid}", String(data->nodeID));
            topic.replace("{key}", String(data->name));
            mqttSend((char *) topic.c_str(), (char *) String(data->value).c_str());
        }
    }

}

void blink(unsigned int time) {
    digitalWrite(LED_PIN, HIGH);
    delay(time);
    digitalWrite(LED_PIN, LOW);
}

// -----------------------------------------------------------------------------
// Hardware
// -----------------------------------------------------------------------------

void hardwareSetup() {
    Serial.begin(SERIAL_BAUD);
    Serial.println();
    Serial.println();
    pinMode(LED_PIN, OUTPUT);
}

void hardwareLoop() {

    // Heartbeat
    static unsigned long last_heartbeat = 0;

    if (millis() - last_heartbeat > HEARTBEAT_INTERVAL) {
        last_heartbeat = millis();
        mqttSend((char *) getSetting("hbTopic", HEARTBEAT_TOPIC).c_str(), (char *) "1");
        #if DEBUG
            Serial.print(F("[BEAT] Free heap: "));
            Serial.println(ESP.getFreeHeap());
        #endif
    }

}

// -----------------------------------------------------------------------------
// Bootstrap methods
// -----------------------------------------------------------------------------

void setup() {
    hardwareSetup();
    settingsSetup();
    otaSetup();
    wifiSetup();
    mqttSetup();
    radioSetup();
    webServerSetup();
}

void loop() {
    hardwareLoop();
    settingsLoop();
    otaLoop();
    wifiLoop();
    mqttLoop();
    radioLoop();
    webServerLoop();
}
