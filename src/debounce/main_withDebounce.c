#ifndef F_CPU
# define F_CPU 16000000
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
// DEBUG
//#include <uart.h>
#include <stdlib.h>
#include "timer.h"

#define TL05_PORT PORTB
#define TL05_INOUT DDRB
#define TL68_PORT PORTC
#define TL68_INOUT DDRC

#define TL05_IN PINB
#define TL68_IN PINC

#define MASK_TL05 0x3f
#define MASK_TL68 0x07

volatile uint8_t deb_vyst;
volatile uint8_t mode_tl;
volatile uint8_t sample;

int main(void)
{
    // DEBUG
    //uart_init(UART_BAUD_SELECT(115200, F_CPU));

    TL05_PORT = TL05_PORT | MASK_TL05;
    TL05_INOUT = TL05_INOUT & ~MASK_TL05;

    TL68_PORT = TL68_PORT | MASK_TL68;
    TL68_INOUT = TL68_INOUT & ~MASK_TL68;

    // nastaveni preruseni
    TIM1_ovf_enable();
    TIM1_ovf_33ms();

    // DEBUG
    //sei();



    while (1)
    {
        /* code */
    }
    
}

ISR(TIMER1_OVF_vect) //60ms
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

    deb_vyst = ~(first | second) & third; // nabezna hrana stisknuteho tlacika

    // debouncing mode (devateho) tlacitka
    third_mode = second_mode;
    second_mode = first_mode;

    first_mode = (TL68_IN & MASK_TL68) >> 2;
    mode_tl = ~(first_mode | second_mode) & third_mode;


    // jednoduchy casovac
    cycle++;
    if (cycle >= 4)
    {
        cycle = 0;
        sample = 1;
    }
    
    /*mode = mode << 1;
    //mode = (mode & ~(1 << 1)) | ((SREG & (1 << 3)) >> 2);
    mode = (mode & ~(1 << 1)) | ((TL68_IN & 0x04) >> 2);
    mode = (mode & ~(1 << 0)) | ((~((mode & 0x02) | (mode & 0x04)) & (mode & 0x08)) & 0x01);*/

    
    ////////// DEBUG
    /*char string[8];
    if (mode_tl > 0)
    {
        //vypis uart mode on
        uart_puts("Mode: ");
        uart_puts("0b");
        itoa(mode_tl, string, 2);
        uart_puts(string);
        uart_puts("\n");
    }
    if (deb_vyst > 0)
    {
        // vypis deb_vystup
        uart_puts("deb_vyst: ");
        uart_puts("0b");
        itoa(deb_vyst, string, 2);
        uart_puts(string);
        uart_puts("\n");
    }
    */
    
}
