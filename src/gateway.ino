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

#include "version.h"
#include "defaults.h"
#include <WebSockets.h>
#include "RFM69Manager.h"

String getSetting(const String& key, String defaultValue = "");
struct _node_t {
  unsigned long count = 0;
  unsigned long missing = 0;
  unsigned long duplicates = 0;
  unsigned char lastPacketID = 0;
};

_node_t nodeInfo[255];

// -----------------------------------------------------------------------------
// Common methods
// -----------------------------------------------------------------------------

void processMessage(packet_t * data) {

    blink(3);

    #if DEBUG
        Serial.print("[MESSAGE]");
        Serial.print(" messageID:");
        Serial.print(data->messageID);
        Serial.print(" senderID:");
        Serial.print(data->senderID);
        Serial.print(" targetID:");
        Serial.print(data->targetID);
        Serial.print(" packetID:");
        Serial.print(data->packetID);
        Serial.print(" name:");
        Serial.print(data->name);
        Serial.print(" value:");
        Serial.print(data->value);
        Serial.print(" rssi:");
        Serial.print(data->rssi);
    #endif

    // Detect duplicates and missing packets
    // packetID==0 means device is not sending packetID info
    if (data->packetID > 0) {

      unsigned char gap = data->packetID - nodeInfo[data->senderID].lastPacketID;

        if (gap == 0) {
            #if DEBUG
                Serial.print(" DUPLICATED");
            #endif
            nodeInfo[data->senderID].duplicates = nodeInfo[data->senderID].duplicates + 1;
            return;
        }

        if ((gap > 1) && (data->packetID > 1)) {
            #if DEBUG
                Serial.print(" MISSING PACKETS!!");
            #endif
            nodeInfo[data->senderID].missing = nodeInfo[data->senderID].missing + gap - 1;
        }

    }

    nodeInfo[data->senderID].lastPacketID = data->packetID;
    nodeInfo[data->senderID].count = nodeInfo[data->senderID].count + 1;

    #if DEBUG
        Serial.println();
    #endif

    // Send info to websocket clients
    char buffer[60];
    sprintf_P(
        buffer,
        PSTR("{'senderID': %u, 'targetID': %u, 'packetID': %u, 'name': '%s', 'value': '%s', 'rssi': %d, 'duplicates': %d, 'missing': %d}"),
        data->senderID, data->targetID, data->packetID, data->name, data->value, data->rssi,
        nodeInfo[data->senderID].duplicates , nodeInfo[data->senderID].missing);
    webSocketSend(buffer);

    // Try to find a matching mapping
    bool found = false;
    unsigned int count = getSetting("mappingCount", "0").toInt();
    for (unsigned int i=0; i<count; i++) {
        if ((getSetting("nodeid" + String(i)) == String(data->senderID)) &&
            (getSetting("key" + String(i)) == data->name)) {
            mqttSend((char *) getSetting("topic" + String(i)).c_str(), (char *) String(data->value).c_str());
            found = true;
            break;
        }
    }

    if (!found) {
        String topic = getSetting("defaultTopic");
        if (topic.length() > 0) {
            topic.replace("{nodeid}", String(data->senderID));
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

    if (mqttConnected()) {
        if ((millis() - last_heartbeat > HEARTBEAT_INTERVAL) || (last_heartbeat == 0)) {
            last_heartbeat = millis();
            mqttSend((char *) getSetting("hbTopic", HEARTBEAT_TOPIC).c_str(), (char *) "1");
            #if DEBUG
                Serial.print(F("[BEAT] Free heap: "));
                Serial.println(ESP.getFreeHeap());
            #endif
        }
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
    webSocketSetup();
}

void loop() {
    hardwareLoop();
    settingsLoop();
    otaLoop();
    wifiLoop();
    mqttLoop();
    radioLoop();
    webServerLoop();
    webSocketLoop();
}
