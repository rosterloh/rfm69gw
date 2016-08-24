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

*/

#ifndef RFM69Manager_h
#define RFM69Manager_h

#include <RFM69.h>
#include <RFM69_ATC.h>
#include <SPI.h>
#include <SPIFlash.h>

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------


#define SPI_CS              SS
#define IRQ_PIN             5
#define IRQ_NUM             5
#define PING_EVERY          3
#define RETRIES             2
#define REQUESTACK          1
//#define ENABLE_ATC

typedef struct {
    uint8_t packetID;
    uint8_t nodeID;
    char * name;
    char * value;
    int16_t rssi;
} packet_t;

typedef void (*TMessageCallback)(packet_t *);

class RFM69Manager: public RFM69_ATC {

    public:

        #ifdef ESP8266
            RFM69Manager(): RFM69_ATC(SPI_CS, IRQ_PIN, false, IRQ_NUM){}
        #else
            RFM69Manager(): RFM69_ATC(){}
        #endif

        bool initialize(uint8_t frequency, uint8_t nodeID, uint8_t networkID, uint8_t gatewayID, const char* key, bool isRFM69HW);
        void onMessage(TMessageCallback fn);
        bool send(uint8_t destinationID, char * name, char * value, uint8_t retries = RETRIES, bool requestACK = REQUESTACK);
        bool send(char * name, char * value, uint8_t retries = RETRIES) {
            return send(_gatewayID, name, value, retries, false);
        }
        bool send(char * name, char * value, bool requestACK = REQUESTACK) {
            return send(_gatewayID, name, value, 0, requestACK);
        }
        void loop();

    private:

        TMessageCallback _callback = NULL;
        uint8_t _gatewayID = 0;
        unsigned char _recieveCount = 0;
        unsigned char _sendCount = 0;
        unsigned int _ackCount = 0;

};

#endif
