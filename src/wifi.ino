/*

ESP69GW
WIFI MODULE

ESP8266 to RFM69 Gateway

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "JustWifi.h"

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------

String getIP() {
    if (WiFi.getMode() == WIFI_AP) {
        return WiFi.softAPIP().toString();
    }
    return WiFi.localIP().toString();
}

String getNetwork() {
    if (WiFi.getMode() == WIFI_AP) {
        return jw.getAPSSID();
    }
    return WiFi.SSID();
}

void wifiDisconnect() {
    jw.disconnect();
}

void resetConnectionTimeout() {
    jw.resetReconnectTimeout();
}

void wifiConfigure() {
    jw.cleanNetworks();
    if (getSetting("ssid0").length() > 0) jw.addNetwork((char *) getSetting("ssid0").c_str(), (char *) getSetting("pass0").c_str());
    if (getSetting("ssid1").length() > 0) jw.addNetwork((char *) getSetting("ssid1").c_str(), (char *) getSetting("pass1").c_str());
    if (getSetting("ssid2").length() > 0) jw.addNetwork((char *) getSetting("ssid2").c_str(), (char *) getSetting("pass2").c_str());
    jw.disconnect();
}

void wifiSetup() {
    jw.setHostname((char *) HOSTNAME);
    jw.scanNetworks(true);
    jw.setAPMode(AP_MODE_ALONE);
    jw.setSoftAP((char *) HOSTNAME, (char *) AP_PASS);
    wifiConfigure();

    // Message callbacks
    jw.onMessage([](justwifi_messages_t code, char * parameter) {

        #ifdef DEBUG_PORT

            if (code == MESSAGE_SCANNING) {
                Serial.printf("[WIFI] Scanning\n");
            }

            if (code == MESSAGE_SCAN_FAILED) {
                Serial.printf("[WIFI] Scan failed\n");
            }

            if (code == MESSAGE_NO_NETWORKS) {
                Serial.printf("[WIFI] No networks found\n");
            }

            if (code == MESSAGE_NO_KNOWN_NETWORKS) {
                Serial.printf("[WIFI] No known networks found\n");
            }

            if (code == MESSAGE_FOUND_NETWORK) {
                Serial.printf("[WIFI] %s\n", parameter);
            }

            if (code == MESSAGE_CONNECTING) {
                Serial.printf("[WIFI] Connecting to %s\n", parameter);
            }

            if (code == MESSAGE_CONNECT_WAITING) {
                // too much noise
            }

            if (code == MESSAGE_CONNECT_FAILED) {
                Serial.printf("[WIFI] Could not connect to %s\n", parameter);
            }

            if (code == MESSAGE_CONNECTED) {
                Serial.printf("[WIFI] MODE STA -------------------------------------\n");
                Serial.printf("[WIFI] SSID %s\n", WiFi.SSID().c_str());
                Serial.printf("[WIFI] IP   %s\n", WiFi.localIP().toString().c_str());
                Serial.printf("[WIFI] MAC  %s\n", WiFi.macAddress().c_str());
                Serial.printf("[WIFI] GW   %s\n", WiFi.gatewayIP().toString().c_str());
                Serial.printf("[WIFI] MASK %s\n", WiFi.subnetMask().toString().c_str());
                Serial.printf("[WIFI] DNS  %s\n", WiFi.dnsIP().toString().c_str());
                Serial.printf("[WIFI] HOST %s\n", WiFi.hostname().c_str());
                Serial.printf("[WIFI] ----------------------------------------------\n");
            }

            if (code == MESSAGE_ACCESSPOINT_CREATED) {
                Serial.printf("[WIFI] MODE AP --------------------------------------\n");
                Serial.printf("[WIFI] SSID %s\n", jw.getAPSSID().c_str());
                Serial.printf("[WIFI] IP   %s\n", WiFi.softAPIP().toString().c_str());
                Serial.printf("[WIFI] MAC  %s\n", WiFi.softAPmacAddress().c_str());
                Serial.printf("[WIFI] ----------------------------------------------\n");
            }

            if (code == MESSAGE_DISCONNECTED) {
                Serial.printf("[WIFI] Disconnected\n");
            }

            if (code == MESSAGE_ACCESSPOINT_CREATING) {
                Serial.printf("[WIFI] Creating access point\n");
            }

            if (code == MESSAGE_ACCESSPOINT_FAILED) {
                Serial.printf("[WIFI] Could not create access point\n");
            }

        #endif

        // Disconnect from MQTT server if no WIFI
        if (code != MESSAGE_CONNECTED) {
            if (mqttConnected()) mqttDisconnect();
        }

        // Configure mDNS
	      if (code == MESSAGE_CONNECTED) {

            if (MDNS.begin((char *) WiFi.hostname().c_str())) {
                MDNS.addService("http", "tcp", 80);
                Serial.printf("[MDNS] OK\n");
            } else {
                Serial.printf("[MDNS] FAIL\n");
            }

        }

    });

}

void wifiLoop() {
    jw.loop();
}
