#include "eeprom.h"


void eeprom_write_byte(uint16_t address, uint8_t data)
// writes one byte to a specific address in EEPROM
{
  twi_start();

  twi_write((EEPROM_ADR<<1) | TWI_WRITE);

  twi_write(address>>8);

  twi_write(address&0xFF);

  twi_write(data);

  twi_stop();
}

uint8_t eeprom_read_byte(uint16_t address)
// read one byte of data from a specific address in EEPROM
{
  twi_start();

  twi_write((EEPROM_ADR<<1) | TWI_WRITE);

  twi_write(address>>8);

  twi_write(address & 0xFF) == 0;
  
  twi_start();

  twi_write((EEPROM_ADR << 1) | TWI_READ); 

  uint8_t data = twi_read(TWI_NACK);

  twi_stop();
  
  return data;
}