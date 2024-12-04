#ifndef F_CPU
# define F_CPU 16000000
#endif

#include <avr/io.h>         // AVR device-specific IO definitions
#include <util/delay.h>     // Adds fuction to delay
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <stdlib.h>         // C library. Needed for number conversions
#include "timer.h"
#include <oled.h>
#include <twi.h>

#include <eeprom.h>
#include <display.h>

#define Xyl_vys PORTD       // output port for xylophone
#define Xyl_inout DDRD      // register for setting output pins

#define TL05_PORT PORTB
#define TL05_INOUT DDRB
#define TL68_PORT PORTC
#define TL68_INOUT DDRC     //registers for buttons

#define TL05_IN PINB
#define TL68_IN PINC

#define MASK_TL05 0x3f
#define MASK_TL68 0x07      //masks for buttons

volatile uint8_t deb_vyst;
volatile uint8_t mode_tl;
volatile uint8_t sample;    //global variables used for interaction of interrupt used for debouncing and main loop

void play(uint8_t);

uint8_t debounce_to_number (uint8_t vst);

void sw_mode(uint8_t mode_sw);

int main(){
  TL05_PORT = TL05_PORT | MASK_TL05;
  TL05_INOUT = TL05_INOUT & ~MASK_TL05;
  TL68_PORT = TL68_PORT | MASK_TL68;
  TL68_INOUT = TL68_INOUT & ~MASK_TL68;     //setting button pins as an pullup input

  Xyl_inout = 0xff;

  play(0x55);

  initSystem();
  sw_mode(1);

  // nastaveni preruseni
  TIM1_ovf_enable();
  TIM1_ovf_33ms();
  sei();

  uint8_t mode = 1;
  uint8_t subloop = 0;
  while (1){
    if(mode_tl > 0){
      if(subloop == 0){
        mode++;                       //jestliže nejsme v podfunkci, pak přepneme mod
        if(mode > 4) mode = 1;
        sw_mode(mode);
        mode_tl = 0;                  //zpracovali jsme mode
      }
    }
    if(deb_vyst > 0){  
      uint8_t track;
      uint16_t addr = 0; 
      uint8_t delka;              //jestliže máme nezpracovanou náběžnou hranu funkčního tlačítka
      switch(mode){                   //dle modu
        case 1:
          play(deb_vyst);             //zahrej notu
          deb_vyst = 0;
          break;
        case 2:
          subloop = 1;                //začni nahrávat
          uint8_t deb_out = debounce_to_number(deb_vyst);
          recording(deb_out);
          addr = 0;
          addr += (deb_out << 8);     //dle čísla tracku najdi adresu, kde je uložena délka nahrávky
          deb_vyst = 0;
          delka = 1;
          while(subloop > 0){
            while(sample == 0){_delay_us(1);} //po uběhnutí času 264ms
            sample = 0;
            displayPosition(delka);           //zobraz zpracování samplu
            play(deb_vyst); 
            eeprom_write_byte(addr + delka, deb_vyst);  //zahrej a ulož noty
            deb_vyst = 0;
            delka++;
            if((delka >= 255) | (mode_tl > 0)){         //když je délka záznamu plná, nebo je ukončeno uživatelem
              mode_tl = 0;
              subloop = 0;
            }
          }
          sw_mode(2);                         //vrátíme se na obrazovku výběru
          eeprom_write_byte(addr, delka);     //zaznamenáme délku v hlavičce záznamu
          _delay_ms(2000);
          break;
        case 3:
          track = debounce_to_number(deb_vyst);
          deb_vyst = 0;
          deleteRecord();                       //ujistíme se, zda mazat
          while ((mode_tl | deb_vyst) == 0) {_delay_us(1);}
          if(mode_tl){
            eeprom_write_byte((track << 8), 0);   //jestliže ano, pak délku v hlavičce nastavíme na 0
            deleted();
            _delay_ms(2000);
          }
          sw_mode(3);                   //vrátíme se obrazovku výběru
          mode_tl = 0;
          deb_vyst = 0;
          break;
        case 4:
          track = debounce_to_number(deb_vyst);
          deb_vyst = 0;
          addr = 0;
          addr = (track << 8);
          delka = eeprom_read_byte(addr);     //získáme z hlavičky záznamu délku záznamu
          uint8_t pozice = 1;
          playing(track);                     //zobrazíme stav přehrávání na displeji
          mode_tl = 0;
          while((mode_tl == 0) & (pozice <= delka)){    //smyčka končí ve chvíli, kdy jsme přehráli nahrávku, nebo ji ukončí uživatel
            if(sample == 1){                            //jestliže je povolení od obsluhy přerušení
              displayPosition(pozice);                  //pak aktualizujeme displej
              uint8_t tones = eeprom_read_byte(addr + pozice);    //a zahrajeme jeden snímek
              play(tones);
              sample = 0;
              pozice++;                 //inkrementujeme ukazatel pozice
            }
          }
          sw_mode(4);
          mode_tl = 0;
          deb_vyst = 0;
          break;
        default:
          break;
      }
    }
  }
}

uint8_t debounce_to_number (uint8_t vst){
  for(uint8_t i = 0; i <= 7; i++){        //pro každý bit osmibitové proměnné
    if(((vst >> i) & 0x01) == 1){         //uděláme masku, než najdeme první kladný
      return i;                           //po-té vrátíme jeho pozici
    }
  }
  return 7;
}

void play(uint8_t tones){             //puts input byte to the output register
  Xyl_vys = Xyl_vys | tones;          //logical one on the tone position is played every call of the function
  _delay_ms(60);                      //we wait for the tone to be made
  Xyl_vys = 0x00;                     //and prepare for another input
}

void sw_mode(uint8_t mode){
  switch(mode){             //ukáže obrazovku, která patří danému módu
    case 1:
      playMode();
      break;
    case 2:
      recordingTable();
      break;
    case 3:
      deletion();
      break;
    case 4:
      replay();
      break;
    default:
      break;
  }
  if(mode != 1){                //a jestliže to mód vyžaduje
    uint8_t delky_zaz[8];
    for(uint8_t i = 0; i < 8; i++){ //pak nahraje délky nahávek v hlavičkách
      delky_zaz[i] = eeprom_read_byte(i << 8);  //z EEPROM paměti
    }
    displayTable(delky_zaz);        //a ty zobrazí v tabulce
  }
}

ISR(TIMER1_OVF_vect) 
{
  static uint8_t first;
  static uint8_t second;
  static uint8_t third;

  static uint8_t first_mode;
  static uint8_t second_mode;
  static uint8_t third_mode;

  static uint8_t cycle;

  // debouncing prvnich osmi tlacitek
  third = second;
  second = first;
  // nahrani z tlacitek
  first = (TL05_IN & MASK_TL05) | ((TL68_IN & MASK_TL68) << 6);

  deb_vyst = deb_vyst | (~(first | second) & third); // nabezna hrana stisknuteho tlacika (jestliže bylo puštené a po-té další dvě obsluhy stlačené)
                                                     // pak nastavíme jeho bit na jedna, po jeho zpracování v mainu ho program v mainu nastaví na nula
  // debouncing mode (devateho) tlacitka
  third_mode = second_mode;
  second_mode = first_mode;

  first_mode = (TL68_IN & MASK_TL68) >> 2;
  mode_tl = mode_tl | (~(first_mode | second_mode) & third_mode);

  // jednoduchy casovac
  cycle++;
  if (cycle >= 8)
  {
    cycle = 0;
    sample = 1;
  }
}
