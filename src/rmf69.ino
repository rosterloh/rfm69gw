/*

ESP69GW
RFM69 MODULE

ESP8266 to RFM69 Gateway

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "RFM69Manager.h"

RFM69Manager radio;

void processMessage(packet_t * data);

// -----------------------------------------------------------------------------
// RFM69
// -----------------------------------------------------------------------------

void radioSetup() {
    delay(10);
    radio.initialize(FREQUENCY, NODEID, NETWORKID, 0, ENCRYPTKEY, IS_RFM69HW);
    radio.onMessage(processMessage);
}

void radioLoop() {
    radio.loop();
}
