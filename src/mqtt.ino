/*

ESP69GW
MQTT MODULE

ESP8266 to RFM69 Gateway

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>

AsyncMqttClient mqtt;

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
    mqtt.publish(topic, MQTT_QOS, MQTT_RETAIN, message);
}

void _mqttOnConnect(bool sessionPresent) {

    DEBUG_MSG("[MQTT] Connected!\n");

    // Send status via webSocket
    wsSend((char *) "{\"mqttStatus\": true}");

    // Say hello and report our IP and VERSION
    mqttSend((char *) MQTT_IP_TOPIC, (char *) getIP().c_str());

}

void _mqttOnDisconnect(AsyncMqttClientDisconnectReason reason) {

    // Send status via webSocket
    wsSend((char *) "{\"mqttStatus\": false}");

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
        mqtt
            .setKeepAlive(MQTT_KEEPALIVE)
            .setCleanSession(false)
            //.setWill("topic/online", 2, true, "no")
            .setClientId(getSetting("hostname", HOSTNAME).c_str());

        if ((user != "") & (pass != "")) {
            DEBUG_MSG(" as user %s.\n", (char *) user.c_str());
            mqtt.setCredentials(user.c_str(), pass.c_str());
        } else {
            DEBUG_MSG(" anonymously\n");
        }

        mqtt.connect();

    }

}

void mqttSetup() {
    mqtt.onConnect(_mqttOnConnect);
    mqtt.onDisconnect(_mqttOnDisconnect);
}

void mqttLoop() {

    static unsigned long lastPeriod = 0;

    if (WiFi.status() == WL_CONNECTED) {

        if (!mqtt.connected()) {

            unsigned long currPeriod = millis() / MQTT_RECONNECT_DELAY;
          	if (currPeriod != lastPeriod) {
          	    lastPeriod = currPeriod;
                mqttConnect();
            }

        }

    }

}
