#ifndef EEPROM_H
#define EEPROM_H
#include "twi.h"
#include <util/delay.h>

#define EEPROM_ADR 0x57

void eeprom_write_byte(uint16_t address, uint8_t data);

uint8_t eeprom_read_byte(uint16_t address);


#endif