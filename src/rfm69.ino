/*

RFM69 MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "RFM69Manager.h"

RFM69Manager radio(SPI_CS, IRQ_PIN, IS_RFM69HW, digitalPinToInterrupt(IRQ_PIN));

void processMessage(packet_t * data);

// -----------------------------------------------------------------------------
// RFM69
// -----------------------------------------------------------------------------

void radioSetup() {
    delay(10);
    radio.initialize(FREQUENCY, NODEID, NETWORKID, ENCRYPTKEY);
    radio.promiscuous(PROMISCUOUS);
    radio.onMessage(processMessage);
}

void radioLoop() {
    radio.loop();
}
