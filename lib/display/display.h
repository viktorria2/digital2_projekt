#ifndef DISPLAY_H
#define DISPLAY_H


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "oled.h"


/* @brief  Display title of recording table */
void recordingTable();

/* @brief  Ask the user for a track to delete  */
void deletion();

/* @brief  Ask the user for pressing a note that should be played  */
void playMode();

/* @brief  Ask the user for the track that should be replayed  */
void replay();

/**
 * @brief  Display playing mode
 * @param  note track that will be replayed
 */
void playing(uint8_t note);

/**
 * @brief  Display recording mode
 * @param  note the segment of eeprom where the recording will be saved
 */
void recording(uint8_t note);

/**
 * @brief  Show how many bytes has been already recorded since the start of recording
 * @param  position how many bytes has been already recorded since the start of recording
 */
void displayPosition(uint8_t position);

/* @brief  show all tracks and length of recordings in each track  */
void displayTable();

/* @brief  ask the user for confirmation of recording deletion */
void deleteRecord();

/* @brief  show the message confirming that the recording has been deleted */
void deleted();

/* @brief  initialize oled display */
void initSystem();

#endif
