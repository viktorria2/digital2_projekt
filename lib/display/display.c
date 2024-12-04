#include "display.h"

void recordingTable() {
    oled_clrscr();  
    oled_gotoxy(0, 0);
    oled_puts("Recording Table");
    oled_display();
}

void deletion() {
    oled_clrscr();  
    oled_gotoxy(0, 0);
    oled_puts("Choose record to delete");
    oled_display();
}

void playMode() {
    oled_clrscr();  
    oled_gotoxy(0, 0);
    oled_puts("Press note to play"); 
    oled_display();
}

void replay() {
    oled_clrscr();  
    oled_gotoxy(0, 0);
    oled_puts("Choose track to play");    
    oled_display();
}

void playing(uint8_t note) {
    char track[3];
    itoa(note,track,10);
    oled_clrscr();  
    oled_gotoxy(0, 0);
    oled_puts("Playing track");
    oled_gotoxy(0,1);
    oled_puts("number ");
    oled_puts(track);
    oled_gotoxy(0,2);
    oled_puts("Press mode to stop");
    oled_gotoxy(0,3);
    oled_puts("stops on position 255");
    oled_display();
}

void recording(uint8_t note) {
    char track[3];
    itoa(note,track,10);
    oled_clrscr();  
    oled_gotoxy(0, 0);
    oled_puts("Recording track");
    oled_gotoxy(0,1);
    oled_puts("number ");
    oled_puts(track);
    oled_gotoxy(0,2);
    oled_puts("Press mode to stop");
    oled_gotoxy(0,3);
    oled_puts("stops on position 255");
    oled_display();
}

void displayPosition(uint8_t position) {
    char track[3];
    itoa(position,track,10);
    oled_gotoxy(0, 4);
    oled_puts("Position: ");
    oled_puts(track);
    oled_display();  
}

// mazani
void deleteRecord() {
    oled_clrscr();  
    oled_gotoxy(0, 0);
    oled_puts("Confirm delete?");
    oled_gotoxy(0, 2);
    oled_puts("Press Mode to delete");
    oled_display();  
}

void deleted() {
    oled_clrscr();  
    oled_gotoxy(0, 0);
    oled_puts("Deleted successfully");
    oled_display(); 
}

void initSystem() {
    oled_init(OLED_DISP_ON);
    oled_clrscr();
    sei();  
}
