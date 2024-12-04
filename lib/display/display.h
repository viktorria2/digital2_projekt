#ifndef DISPLAY_H
#define DISPLAY_H


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "oled.h"

void recordingTable();

void deletion();

void playMode();

void replay();

void playing(uint8_t note);

void recording(uint8_t note);

void displayPosition(uint8_t position);

void displayTable();

void deleteRecord();

void deleted();

void initSystem();

#endif