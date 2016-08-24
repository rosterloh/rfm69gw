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
// WebServer
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

bool handleFileRead(String path) {

    #if DEBUG
        Serial.print(F("[WEBSERVER] Request: "));
        Serial.println(path);
    #endif

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

void handleGet() {

    #if DEBUG
        Serial.println("[WEBSERVER] Request: /get");
    #endif

    char buffer[64];
    sprintf(buffer, "%s %s", APP_NAME, APP_VERSION);

    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["app"] = buffer;
    root["hostname"] = getValue("hostname", HOSTNAME);
    root["network"] = getNetwork();
    root["ip"] = getIP();
    root["mqttStatus"] = mqtt.connected() ? "1" : "0";
    root["mqttServer"] = getValue("mqttServer", MQTT_SERVER);
    root["mqttPort"] = getValue("mqttPort", String(MQTT_PORT));
    root["mqttUser"] = getValue("mqttUser", MQTT_USER);
    root["mqttPassword"] = getValue("mqttPassword", MQTT_PASS);
    root["ipTopic"] = getValue("ipTopic", IP_TOPIC);
    root["hbTopic"] = getValue("hbTopic", HEARTBEAT_TOPIC);
    root["defaultTopic"] = getValue("defaultTopic", DEFAULT_TOPIC);

    JsonArray& wifi = root.createNestedArray("wifi");
    for (byte i=0; i<3; i++) {
        JsonObject& network = wifi.createNestedObject();
        network["ssid"] = getValue("ssid" + String(i));
        network["pass"] = getValue("pass" + String(i));
    }

    JsonArray& mappings = root.createNestedArray("mapping");
    byte mappingCount = getValue("mappingCount", "0").toInt();
    for (byte i=0; i<mappingCount; i++) {
        JsonObject& mapping = mappings.createNestedObject();
        mapping["nodeid"] = getValue("nodeid" + String(i));
        mapping["key"] = getValue("key" + String(i));
        mapping["topic"] = getValue("topic" + String(i));
    }

    String output;
    root.printTo(output);
    server.send(200, "text/json", output);

}

void handlePost() {

    #if DEBUG
        Serial.println(F("[WEBSERVER] Request: /post"));
    #endif

    unsigned int mappingCount = getValue("mappingCount", "0").toInt();
    unsigned int network = 0;
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

        setValue(key, value);

    }

    // delete remaining mapping
    for (unsigned int i=mapping; i<mappingCount; i++) {
        delValue("nodeid" + String(i));
        delValue("key" + String(i));
        delValue("topic" + String(i));
    }

    String value = String(mapping);
    setValue("mappingCount", value);
    saveSettings();

    server.send(202, "text/json", "{}");

    // Disconnect from current WIFI network if it's not the first on the list
    // wifiLoop will take care of the reconnection
    if (getNetwork() != getValue("ssid0")) wifiDisconnect();

}

void webServerSetup() {

    SPIFFS.begin();

    // Configuration page
    server.on("/get", HTTP_GET, handleGet);
    server.on("/post", HTTP_POST, handlePost);

    // Anything else
    server.onNotFound([]() {

        // Hidden files
        #ifndef DEBUG
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
