/*

ESP69GW
WEBSOCKETS MODULE

ESP8266 to RFM69 Gateway

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <WebSocketsServer.h>
#include <Hash.h>
#include <ArduinoJson.h>

WebSocketsServer webSocket = WebSocketsServer(81);

// -----------------------------------------------------------------------------
// WEBSOCKETS
// -----------------------------------------------------------------------------

bool webSocketSend(char * payload) {
    //DEBUG_MSG("[WEBSOCKET] Broadcasting '%s'\n", payload);
    webSocket.broadcastTXT(payload);
}

bool webSocketSend(uint8_t num, char * payload) {
    //DEBUG_MSG("[WEBSOCKET] Sending '%s' to #%d\n", payload, num);
    webSocket.sendTXT(num, payload);
}

void webSocketStart(uint8_t num) {

    char app[64];
    sprintf(app, "%s %s", APP_NAME, APP_VERSION);

    char chipid[6];
    sprintf(chipid, "%06X", ESP.getChipId());

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["app"] = app;
    root["hostname"] = getSetting("hostname", HOSTNAME);
    root["chipid"] = chipid;
    root["mac"] = WiFi.macAddress();
    root["device"] = String(DEVICE);
    root["network"] = getNetwork();
    root["ip"] = getIP();
    root["mqttStatus"] = mqttConnected() ? "1" : "0";
    root["mqttServer"] = getSetting("mqttServer", MQTT_SERVER);
    root["mqttPort"] = getSetting("mqttPort", String(MQTT_PORT));
    root["mqttUser"] = getSetting("mqttUser");
    root["mqttPassword"] = getSetting("mqttPassword");
    root["ipTopic"] = getSetting("ipTopic", MQTT_IP_TOPIC);
    root["hbTopic"] = getSetting("hbTopic", MQTT_HEARTBEAT_TOPIC);
    root["defaultTopic"] = getSetting("defaultTopic", MQTT_DEFAULT_TOPIC);

    JsonArray& wifi = root.createNestedArray("wifi");
    for (byte i=0; i<3; i++) {
        JsonObject& network = wifi.createNestedObject();
        network["ssid"] = getSetting("ssid" + String(i));
        network["pass"] = getSetting("pass" + String(i));
    }

    JsonArray& mappings = root.createNestedArray("mapping");
    byte mappingCount = getSetting("mappingCount", "0").toInt();
    for (byte i=0; i<mappingCount; i++) {
        JsonObject& mapping = mappings.createNestedObject();
        mapping["nodeid"] = getSetting("nodeid" + String(i));
        mapping["key"] = getSetting("key" + String(i));
        mapping["topic"] = getSetting("topic" + String(i));
    }

    String output;
    root.printTo(output);
    webSocket.sendTXT(num, (char *) output.c_str());

}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            DEBUG_MSG("[WEBSOCKET] #%u disconnected\n", num);
            break;
        case WStype_CONNECTED:
            #if DEBUG_PORT
                {
                    IPAddress ip = webSocket.remoteIP(num);
                    DEBUG_MSG("[WEBSOCKET] #%u connected, ip: %d.%d.%d.%d, url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                }
            #endif
            webSocketStart(num);
            break;
        case WStype_TEXT:
            DEBUG_MSG("[WEBSOCKET] #%u sent: %s\n", num, payload);
            break;
        case WStype_BIN:
            DEBUG_MSG("[WEBSOCKET] #%u sent binary length: %u\n", num, length);
            break;
    }

}

void webSocketSetup() {
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void webSocketLoop() {
    webSocket.loop();
}
