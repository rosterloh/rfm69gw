/*

ESP69GW
MQTT MODULE

ESP8266 to RFM69 Gateway

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

WiFiClient client;
PubSubClient mqtt(client);
boolean mqttStatus = false;

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

bool mqttConnected() {
    return mqtt.connected();
}

void mqttDisconnect() {
    mqtt.disconnect();
}

void mqttSend(char * topic, char * message) {
    if (!mqtt.connected()) return;
    DEBUG_MSG("[MQTT] Sending %s %s\n", topic, message);
    mqtt.publish(topic, message, MQTT_RETAIN);
}

void mqttConnect() {

    if (!mqtt.connected()) {

        String host = getSetting("mqttServer", MQTT_SERVER);
        String port = getSetting("mqttPort", String(MQTT_PORT));
        String user = getSetting("mqttUser");
        String pass = getSetting("mqttPassword");

        if (host.length() == 0) return;

        DEBUG_MSG("[MQTT] Connecting to broker at %s", (char *) host.c_str());
        mqtt.setServer(host.c_str(), port.toInt());

        if ((user != "") & (pass != "")) {
            DEBUG_MSG(" as user %s: ", (char *) user.c_str());
            mqtt.connect(getSetting("hostname", HOSTNAME).c_str(), user.c_str(), pass.c_str());
        } else {
            DEBUG_MSG(" anonymously: ");
            mqtt.connect(getSetting("hostname", HOSTNAME).c_str());
        }

        if (mqtt.connected()) {

            DEBUG_MSG("connected!\n");

            mqttStatus = true;

            // Send status via webSocket
            webSocketSend((char *) "{\"mqttStatus\": true}");

            // Say hello and report our IP
            mqttSend((char *) getSetting("ipTopic", MQTT_IP_TOPIC).c_str(), (char *) getIP().c_str());

        } else {

            DEBUG_MSG("failed (rc=%d)\n", mqtt.state());

        }
    }

}

void mqttSetup() {
    //mqtt.setCallback(mqttCallback);
}

void mqttLoop() {

    static unsigned long lastPeriod = 0;

    if (WiFi.status() == WL_CONNECTED) {

        if (!mqtt.connected()) {

            if (mqttStatus) {
                webSocketSend((char *) "{\"mqttStatus\": false}");
                mqttStatus = false;
            }

          	unsigned long currPeriod = millis() / MQTT_RECONNECT_DELAY;
          	if (currPeriod != lastPeriod) {
          	    lastPeriod = currPeriod;
                mqttConnect();
            }

        }

        if (mqtt.connected()) mqtt.loop();
        
    }

}
