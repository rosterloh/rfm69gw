/*

ESP69GW
WEBSERVER MODULE

ESP8266 to RFM69 Gateway

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "FS.h"

ESP8266WebServer server(80);

// -----------------------------------------------------------------------------
// WEBSERVER
// -----------------------------------------------------------------------------

String getContentType(String filename) {
    if (server.hasArg("download")) return "application/octet-stream";
    else if (filename.endsWith(".htm")) return "text/html";
    else if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    else if (filename.endsWith(".xml")) return "text/xml";
    else if (filename.endsWith(".pdf")) return "application/x-pdf";
    else if (filename.endsWith(".zip")) return "application/x-zip";
    else if (filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}

void handleReconnect() {
    DEBUG_MSG("[WEBSERVER] Request: /reconnect\n");
    wifiDisconnect();
}

void handleReset() {
    DEBUG_MSG("[WEBSERVER] Request: /reset\n");
    ESP.reset();
}

bool handleFileRead(String path) {

    DEBUG_MSG("[WEBSERVER] Request: %s\n", (char *) path.c_str());

    if (path.endsWith("/")) path += "index.html";
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz)) path = pathWithGz;

    if (SPIFFS.exists(path)) {
        File file = SPIFFS.open(path, "r");
        size_t sent = server.streamFile(file, contentType);
        size_t contentLength = file.size();
        file.close();
        return true;
    }

    return false;

}

void handleSave() {

    DEBUG_MSG("[WEBSERVER] Request: /save\n");

    bool dirty = false;
    bool dirtyMQTT = false;

    unsigned int network = 0;
    unsigned int mappingCount = getSetting("mappingCount", "0").toInt();
    unsigned int mapping = 0;

    for (unsigned int i=0; i<server.args(); i++) {

        String key = server.argName(i);
        String value = server.arg(i);

        if (key == "ssid") {
            key = key + String(network);
        }
        if (key == "pass") {
            key = key + String(network);
            ++network;
        }
        if (key == "nodeid") {
            if (value == "") break;
            key = key + String(mapping);
        }
        if (key == "key") {
            key = key + String(mapping);
        }
        if (key == "topic") {
            key = key + String(mapping);
            ++mapping;
        }

        if (value != getSetting(key)) {
            setSetting(key, value);
            dirty = true;
            if (key.startsWith("mqtt")) dirtyMQTT = true;
        }

    }

    server.send(202, "text/json", "{}");

    // delete remaining mapping
    for (unsigned int i=mapping; i<mappingCount; i++) {
        delSetting("nodeid" + String(i));
        delSetting("key" + String(i));
        delSetting("topic" + String(i));
        dirty = true;
    }

    String value = String(mapping);
    setSetting("mappingCount", value);

    if (dirty) {
        saveSettings();
    }

    // Reconfigure networks
    wifiDisconnect();

    // Check if we should reconigure MQTT connection
    if (dirtyMQTT) {
        mqttDisconnect();
    }

}

void webServerSetup() {

    //SPIFFS.begin();

    // Routes
    server.on("/reconnect", HTTP_GET, handleReconnect);
    server.on("/reset", HTTP_GET, handleReset);
    server.on("/save", HTTP_POST, handleSave);

    // Anything else
    server.onNotFound([]() {

        // Hidden files
        #ifndef DEBUG_PORT
            if (server.uri().startsWith("/.")) {
                server.send(403, "text/plain", "Forbidden");
                return;
            }
        #endif

        // Existing files in SPIFFS
        if (!handleFileRead(server.uri())) {
            server.send(404, "text/plain", "NotFound");
            return;
        }

    });

    // Run server
    server.begin();

}

void webServerLoop() {
    server.handleClient();
}
