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

    #if DEBUG
        Serial.print(F("[MQTT] Sending "));
        Serial.print(topic);
        Serial.print(F(" "));
        Serial.println(message);
    #endif

    mqtt.publish(topic, message, MQTT_RETAIN);

}

void mqttConnect() {

    if (!mqtt.connected()) {

        String host = getSetting("mqttServer", MQTT_SERVER);
        String port = getSetting("mqttPort", String(MQTT_PORT));
        String user = getSetting("mqttUser");
        String pass = getSetting("mqttPassword");

        #if DEBUG
            Serial.print(F("[MQTT] Connecting to broker at "));
            Serial.print(host);
        #endif

        mqtt.setServer(host.c_str(), port.toInt());

        if ((user != "") & (pass != "")) {
            #if DEBUG
                Serial.print(F(" as user "));
                Serial.print(user);
                Serial.print(F(": "));
            #endif
            mqtt.connect(getSetting("hostname", HOSTNAME).c_str(), user.c_str(), pass.c_str());
        } else {
            #if DEBUG
                Serial.print(F(" anonymously: "));
            #endif
            mqtt.connect(getSetting("hostname", HOSTNAME).c_str());
        }

        if (mqtt.connected()) {

            #if DEBUG
                Serial.println(F("connected!"));
            #endif

            // Say hello and report our IP
            mqttSend((char *) getSetting("ipTopic", IP_TOPIC).c_str(), (char *) WiFi.localIP().toString().c_str());

        } else {

            #if DEBUG
                Serial.print(F("failed, rc="));
                Serial.println(mqtt.state());
            #endif

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
        	unsigned long currPeriod = millis() / MQTT_RECONNECT_DELAY;
        	if (currPeriod != lastPeriod) {
        	    lastPeriod = currPeriod;
                mqttConnect();
            }
        }
        if (mqtt.connected()) mqtt.loop();
    }

}
