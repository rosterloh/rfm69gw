/*

ESP69GW
WIFI MODULE

ESP8266 to RFM69 Gateway

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "JustWifi.h"

JustWifi jw;

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------

String getIP() {
    return jw.getIP();
}

String getNetwork() {
    return jw.getNetwork();
}

void wifiDisconnect() {
    jw.disconnect();
}

void wifiSetup() {

    // Message callbacks
    jw.onMessage([](justwifi_messages_t code, char * parameter) {

        // Disconnect from MQTT server if no WIFI
        if (code != MESSAGE_CONNECTED) {
            if (mqtt.connected()) mqtt.disconnect();
        }

        #if DEBUG

            if (code == MESSAGE_AUTO_NOSSID) {
                Serial.println("[WIFI] No information about the last successful network");
            }

            if (code == MESSAGE_AUTO_CONNECTING) {
                Serial.print("[WIFI] Connecting to last successful network: ");
                Serial.println(parameter);
            }

            if (code == MESSAGE_AUTO_FAILED) {
                Serial.println("[WIFI] Could not connect to last successful network");
            }

            if (code == MESSAGE_CONNECTING) {
                Serial.print("[WIFI] Connecting to ");
                Serial.println(parameter);
            }

            if (code == MESSAGE_CONNECT_WAITING) {
                //
            }

            if (code == MESSAGE_CONNECT_FAILED) {
                Serial.print("[WIFI] Could not connect to ");
                Serial.println(parameter);
            }

            if (code == MESSAGE_CONNECTED) {
                Serial.print("[WIFI] Connected to ");
                Serial.print(jw.getNetwork());
                Serial.print(" with IP ");
                Serial.println(jw.getIP());
            }

            if (code == MESSAGE_DISCONNECTED) {
                Serial.println("[WIFI] Disconnected");
            }

            if (code == MESSAGE_ACCESSPOINT_CREATING) {
                Serial.println("[WIFI] Creating access point");
            }

            if (code == MESSAGE_ACCESSPOINT_CREATED) {
                Serial.print("[WIFI] Access point created with SSID ");
                Serial.print(jw.getNetwork());
                Serial.print(" and IP ");
                Serial.println(jw.getIP());
            }

            if (code == MESSAGE_ACCESSPOINT_FAILED) {
                Serial.println("[WIFI] Could not create access point");
            }

        #endif

    });

}

bool wifiAP() {
    //jw.disconnect();
    return jw.startAP((char *) HOSTNAME, (char *) AP_PASS);
}

void wifiLoop() {

    jw.loop();

    // Check disconnection
    if ((!jw.connected()) && (jw.getMode() != MODE_ACCESS_POINT)) {

        jw.cleanNetworks();
        if (getValue("ssid0").length() > 0) jw.addNetwork((char *) getValue("ssid0").c_str(), (char *) getValue("pass0").c_str());
        if (getValue("ssid1").length() > 0) jw.addNetwork((char *) getValue("ssid1").c_str(), (char *) getValue("pass1").c_str());
        if (getValue("ssid2").length() > 0) jw.addNetwork((char *) getValue("ssid2").c_str(), (char *) getValue("pass2").c_str());

        // Connecting
        if (!jw.autoConnect()) {
            if (!jw.connect()) {
                if (!wifiAP()) {
                    #if DEBUG
                        Serial.println("[WIFI] Could not start any wifi interface!");
                    #endif
                }
            }
        }

    }
}
