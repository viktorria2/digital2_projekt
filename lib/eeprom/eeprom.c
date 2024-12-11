#include "eeprom.h"


void eeprom_write_byte(uint16_t address, uint8_t data)
// write one byte to a specific address in EEPROM
{
  twi_start(); // send start condition

  twi_write((EEPROM_ADR<<1) | TWI_WRITE); // shift to the left to make space for the R/W bit and set R/W bit

  twi_write(address>>8); // send MSB of address

  twi_write(address&0xFF); // send LSB of address

  twi_write(data); // send byte of data

  twi_stop(); // send stop condition
}

uint8_t eeprom_read_byte(uint16_t address)
// read one byte of data from a specific address in EEPROM
{
  // write dummy byte to load in the data word address
  twi_start();

  twi_write((EEPROM_ADR<<1) | TWI_WRITE);

  twi_write(address>>8);

  twi_write(address & 0xFF) == 0;
  
  twi_start(); // send start condition again

  twi_write((EEPROM_ADR << 1) | TWI_READ); 

  uint8_t data = twi_read(TWI_NACK);

  twi_stop();
  
  return data;
}
