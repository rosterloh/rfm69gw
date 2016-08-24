/*

Radio

RFM69 Radio Manager for ESP8266
Based on sample code by Felix Rusu - http://LowPowerLab.com/contact
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

Requires encapsulating any reference to SPCR and SPSR in SPIFlash.cpp
in an #if clause like this:

#if defined(SPCR) & defined(SPSR)
...
#endif

*/

#include <RFM69.h>
#include <RFM69_ATC.h>
#include <SPI.h>
#include <SPIFlash.h>
#include "RFM69Manager.h"

bool RFM69Manager::initialize(uint8_t frequency, uint8_t nodeID, uint8_t networkID, uint8_t gatewayID, const char* key, bool isRFM69HW) {

    _gatewayID = gatewayID;
    bool ret = RFM69_ATC::initialize(frequency, nodeID, networkID);
    RFM69_ATC::encrypt(key);
    if (isRFM69HW) RFM69_ATC::setHighPower();
    #ifndef IS_GATEWAY
        RFM69_ATC::enableAutoPower(-70);
    #endif

    char buff[50];
    sprintf(buff, "[RADIO] Listening at %d Mhz...", frequency == RF69_433MHZ ? 433 : frequency == RF69_868MHZ ? 868 : 915);
    Serial.println(buff);
    Serial.println(F("[RADIO] RFM69_ATC Enabled (Auto Transmission Control)"));

    return ret;

}

void RFM69Manager::onMessage(TMessageCallback fn) {
    _callback = fn;
}

void RFM69Manager::loop() {

    if (RFM69_ATC::receiveDone()) {

        if (_callback != NULL) {

            ++_recieveCount;

            char buffer[RFM69_ATC::DATALEN+1];
            strncpy(buffer, (const char *) RFM69_ATC::DATA, RFM69_ATC::DATALEN);
            buffer[RFM69_ATC::DATALEN] = 0;

            uint8_t parts = 1;
            uint8_t packetID = 0;
            for (uint8_t i=0; i<RFM69_ATC::DATALEN; i++) {
                if (buffer[i] == ':') ++parts;
            }
            char * name = strtok(buffer, ":");
            char * value = strtok(NULL, ":");
            if (parts > 2) {
                char * packet = strtok(NULL, ":");
                packetID = atoi(packet);
            }

            packet_t data;
            data.packetID = packetID;
            data.nodeID = RFM69_ATC::SENDERID;
            data.name = name;
            data.value = value;
            data.rssi = RFM69_ATC::RSSI;

            _callback(&data);

        }

        if (RFM69_ATC::ACKRequested()) {

            byte theNodeID = RFM69_ATC::SENDERID;
            RFM69_ATC::sendACK();

            #ifdef IS_GATEWAY

                // When a node requests an ACK, respond to the ACK
                // and also send a packet requesting an ACK (every PING_EVERY)
                // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
                if (_ackCount++ % PING_EVERY == 0) {

                    //need this when sending right after reception .. ?
                    delay(3);

                    // 0 = only 1 attempt, no retries
                    RFM69_ATC::sendWithRetry(theNodeID, "ACK TEST", 8, 0);

                }

            #endif


        }

    }

}

bool RFM69Manager::send(uint8_t destinationID, char * name, char * value, uint8_t retries, bool requestACK) {

    char message[30];
    if (++_sendCount == 0) _sendCount = 1;
    sprintf(message, "%s:%s:%d", name, value, _sendCount);
    Serial.print(F("[RADIO] Sending: "));
    Serial.print(message);

    bool ret = true;
    if (retries > 0) {
        ret = RFM69_ATC::sendWithRetry(destinationID, message, strlen(message), retries);
    } else {
        RFM69_ATC::send(destinationID, message, strlen(message), requestACK);
    }

    if (ret) {
        Serial.println(" OK");
    } else {
        Serial.println(" KO");
    }

}
