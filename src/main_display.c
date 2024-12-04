#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "oled.h"

// nastaveni pinu pro kazdou notu
#define C4_PIN PD2    
#define D4_PIN PD3    
#define E4_PIN PD4   
#define F4_PIN PD5   
#define G4_PIN PD6    
#define A4_PIN PD7    
#define B4_PIN PB0    

// buzzer pin - buzzer se pouziva pro generace zvukovych signalu na mikrokonroleru, zvukovy vystup
#define BUZZER_PIN PB1

// nastaveni frekvenci na noty
#define C4_FREQ 262
#define D4_FREQ 294
#define E4_FREQ 330
#define F4_FREQ 349
#define G4_FREQ 392
#define A4_FREQ 440
#define B4_FREQ 466

// urceni modu a noty
volatile int mode = 0;  
volatile char note = ' ';  

// tabulka
#define MAX_RECORDS 8
char noteRecords[MAX_RECORDS][10];  

// vybrana bunka
volatile uint8_t currentCell = 0;  // index vybrané bunky
volatile uint8_t deleteMode = 0;   // mode ke smazaní bunky

// inicializace tlacitek
void initButtons() {
    DDRD &= ~((1 << C4_PIN) | (1 << D4_PIN) | (1 << E4_PIN) | (1 << F4_PIN) | (1 << G4_PIN) | (1 << A4_PIN) | (1 << B4_PIN)); // piny jako vstupy
    PORTD |= (1 << C4_PIN) | (1 << D4_PIN) | (1 << E4_PIN) | (1 << F4_PIN) | (1 << G4_PIN) | (1 << A4_PIN);  // nechava port na HIGH je-li nevyuzita
    PORTB |= (1 << B4_PIN);  // nechava port na HIGH je-li nevyuzit PB0
}

// buzzer inicializace
void initBuzzer() {
    DDRB |= (1 << BUZZER_PIN);  // PB1 je vystupem
}

// generovani tonu
void playTone(uint16_t frequency) {
    uint16_t period = 1000000 / frequency;  
    for (int i = 0; i < 1000; i++) {  
        PORTB |= (1 << BUZZER_PIN);  // buzzer on
        _delay_us(period / 2);
        PORTB &= ~(1 << BUZZER_PIN); // buzzer off
        _delay_us(period / 2);
    }
}

// zobrazeni tabulky
void displayTable() {
    oled_clrscr();  
    oled_gotoxy(0, 0);
    oled_puts("Recording Table");
    
    for (uint8_t i = 0; i < MAX_RECORDS; i++) {
        uint8_t row = i / 4;  
        uint8_t col = i % 4;
        
        oled_gotoxy(col * 32, row + 1);  
        oled_putc('0' + (i + 1));  
        oled_puts(": ");
        oled_puts(noteRecords[i]);
    }
    oled_display();  
}

// zarazeni not do bunky
void recordNote() {
    if (note != ' ' && currentCell < MAX_RECORDS) {
        uint8_t len = strlen(noteRecords[currentCell]);
        if (len < 9) {  
            noteRecords[currentCell][len] = note;
            noteRecords[currentCell][len + 1] = '\0';  
        }
    }
}

// vyber bunky
void navigateCells() {
    // navagaci pomoci PD6 nahoru a PD7 dolu
    if (!(PIND & (1 << PD6)) && currentCell > 0) {  
        currentCell--;
        _delay_ms(200);  
    }
    if (!(PIND & (1 << PD7)) && currentCell < MAX_RECORDS - 1) {  
        currentCell++;
        _delay_ms(200);  
    }
}

// kontrola stiskle noty
void checkButtonsForRecording() {
    if (!(PIND & (1 << C4_PIN))) {  
        note = 'C';
        playTone(C4_FREQ);
        recordNote();
    } else if (!(PIND & (1 << D4_PIN))) {  
        note = 'D';
        playTone(D4_FREQ);
        recordNote();
    } else if (!(PIND & (1 << E4_PIN))) {  
        note = 'E';
        playTone(E4_FREQ);
        recordNote();
    } else if (!(PIND & (1 << F4_PIN))) { 
        note = 'F';
        playTone(F4_FREQ);
        recordNote();
    } else if (!(PIND & (1 << G4_PIN))) {  
        note = 'G';
        playTone(G4_FREQ);
        recordNote();
    } else if (!(PIND & (1 << A4_PIN))) {  
        note = 'A';
        playTone(A4_FREQ);
        recordNote();
    } else if (!(PINB & (1 << B4_PIN))) {  
        note = 'B';
        playTone(B4_FREQ);
        recordNote();
    }
}

// mazani
void deleteRecord() {
    if (deleteMode && currentCell < MAX_RECORDS) {
        oled_clrscr();  
        oled_gotoxy(0, 0);
        oled_puts("Confirm delete?");
        oled_gotoxy(0, 2);
        oled_puts("Press Yes to delete");
        oled_puts("or No to cancel");
        
        if (!(PIND & (1 << PD6))) {  // ano PD6
            memset(noteRecords[currentCell], 0, sizeof(noteRecords[currentCell]));  
            deleteMode = 0;  
            _delay_ms(200);  
        }
        if (!(PIND & (1 << PD7))) {  // ne PD7
            deleteMode = 0;  
            _delay_ms(200);  
        }
    }
}

// hrani zapsaneho
void playRecord(uint8_t cell) {
    if (cell < MAX_RECORDS) {
        oled_clrscr(); 
        oled_gotoxy(0, 0);
        oled_puts("Playing Record: ");
        oled_puts(noteRecords[cell]);  // zobrazeni not ktere zahrajeme
        
        for (int i = 0; noteRecords[cell][i] != '\0'; i++) {
            switch (noteRecords[cell][i]) {
                case 'C':
                    playTone(C4_FREQ);
                    break;
                case 'D':
                    playTone(D4_FREQ);
                    break;
                case 'E':
                    playTone(E4_FREQ);
                    break;
                case 'F':
                    playTone(F4_FREQ);
                    break;
                case 'G':
                    playTone(G4_FREQ);
                    break;
                case 'A':
                    playTone(A4_FREQ);
                    break;
                case 'B':
                    playTone(B4_FREQ);
                    break;
            }
        }
    }
}

void initSystem() {
    oled_init(OLED_DISP_ON);
    oled_clrscr();
    initButtons();
    initBuzzer();
    sei();  
}

int main(void) {
    initSystem();  

    while (1) {
        displayTable();  
        navigateCells();  
        checkButtonsForRecording();  
        deleteRecord();  
        _delay_ms(200);  
    }

    return 0;
}
