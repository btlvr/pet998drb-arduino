// Brett Leaver
// May 2017
// Arduino code for controlling the Petrainer PET998DRB
// Uses https://www.sparkfun.com/products/10534
// More information at brettleaver.com/collar

#include "Arduino.h"

#define TX_PIN 10

#define BIT_DURATION 640
#define DELIMITER_DURATION 190
#define PREAMBLE_HIGH_DURATION 1500
#define PREAMBLE_LOW_DURATION 750

enum CHANNEL {CHANNEL_1, CHANNEL_2};
enum TYPE {SHOCK, BUZZ, BEEP, LIGHT};

void tx_delim() {
    digitalWrite(TX_PIN,LOW);
    delayMicroseconds(DELIMITER_DURATION);
    digitalWrite(TX_PIN,HIGH);
    delayMicroseconds(DELIMITER_DURATION);
}

void tx_bit(bool value) {
    digitalWrite(TX_PIN,value);
    delayMicroseconds(BIT_DURATION);
    tx_delim();
}

void tx_preamble() {
    digitalWrite(TX_PIN,HIGH);
    delayMicroseconds(PREAMBLE_HIGH_DURATION);
    digitalWrite(TX_PIN,LOW);
    delayMicroseconds(PREAMBLE_LOW_DURATION);
    tx_delim();
}

void tx_single_packet(CHANNEL channel,
                      TYPE type,
                      byte power,
                      byte ID1,
                      byte ID2) {
    byte bytes[5];    
    byte shift = 1;
    if (channel == CHANNEL::CHANNEL_1)
        bytes[0] = 0x80;
    if (channel == CHANNEL::CHANNEL_2)
        bytes[0] = 0xF0;
    switch(type) {
        case TYPE::SHOCK:
        bytes[0] += 0x01;
        break;
    case TYPE::BUZZ:
        bytes[0] += 0x02;
        break;
    case TYPE::BEEP:
        bytes[0] += 0x04;
        break;
    case TYPE::LIGHT:
        bytes[0] += 0x08;
        break;
    }
    bytes[1] = ID1;
    bytes[2] = ID2;
    bytes[3] = power%101;
    switch(type) {
        case TYPE::SHOCK:
            bytes[4] = 0x70;
            break;
        case TYPE::BUZZ:
            bytes[4] = 0xB0;
            break;
        case TYPE::BEEP:
            bytes[4] = 0xD0;
            break;
        case TYPE::LIGHT:
            bytes[4] = 0xE0;
            break;
    }
    if (channel == CHANNEL::CHANNEL_1)
        bytes[4] |= 0x0e;

    tx_preamble();
    for (byte b : bytes) {
        for (int i = 0; i < 8; i++) {   
            bool value = b & (1 << 7);
            b <<= shift;
            tx_bit(value);
        }
    }
    digitalWrite(TX_PIN,LOW);
    delayMicroseconds(BIT_DURATION*2);
}

void tx_packet(CHANNEL channel,
               TYPE type,
               byte power,
               byte ID1,
               byte ID2) {
    for (int i = 0; i < 8; i++)
        tx_single_packet(channel, type, power, ID1, ID2);
}

void setup() {
    tx_packet(CHANNEL::CHANNEL_1, TYPE::BUZZ, 100, 0x20, 0x89);
}

void loop() {
	//
}
