/*

ESP69GW
WEBSOCKETS MODULE

ESP8266 to RFM69 Gateway

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <WebSocketsServer.h>
#include <Hash.h>

WebSocketsServer webSocket = WebSocketsServer(81);

// -----------------------------------------------------------------------------
// WEBSOCKETS
// -----------------------------------------------------------------------------

bool webSocketSend(char * payload) {
    webSocket.broadcastTXT(payload);
}

bool webSocketSend(uint8_t num, char * payload) {
    webSocket.sendTXT(num, payload);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            #if DEBUG
                Serial.printf("[WEBSOCKET] #%u disconnected\n", num);
            #endif
            break;
        case WStype_CONNECTED:
            #if DEBUG
                {
                    IPAddress ip = webSocket.remoteIP(num);
                    Serial.printf("[WEBSOCKET] #%u connected, ip: %d.%d.%d.%d, url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                }
            #endif
            webSocket.sendTXT(num, "Connected");
            break;
        case WStype_TEXT:
            #if DEBUG
                Serial.printf("[WEBSOCKET] #%u sent: %s\n", num, payload);
            #endif
            break;
        case WStype_BIN:
            #if DEBUG
                Serial.printf("[WEBSOCKET] #%u sent binary length: %u\n", num, length);
            #endif
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
