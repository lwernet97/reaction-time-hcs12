#include <hidef.h>
#include "derivative.h"
#include <stdlib.h>  // for rand and srand

// --- Pin Masks ---
#define SW5_MASK  0x01  // PH0  SW5 (Start)
#define SW2_MASK  0x08  // PH3  SW2 (Stop)
#define LED0_MASK 0x01  // PTB0 LED
#define BUZZER_MASK 0x10 // PTP4 (OC5)

// --- Globals ---
unsigned int T1, Tstop, result;
unsigned char r100, r10, r1;

// --- Function Prototypes ---
void SerCom0(unsigned char data);
void SCI_OutString(char *pt);
void MSDelay(unsigned int t);
void start(void);
void stop(void);
void calc(void);
void done(void);
void buzzer_on(void);
void buzzer_off(void);

void main(void) {
  // === Clock Setup (24 MHz) ===
  SYNR = 2;
  REFDV = 0;
  PLLCTL = 0x60;
  while (!(CRGFLG & 0x08));
  CLKSEL = 0x80;

  // === Port Setup ===
  DDRB = 0xFF;                        // LED output
  DDRP = 0xFF;                        // Buzzer output
  DDRH &= ~(SW5_MASK | SW2_MASK);     // PH0 and PH3 as input
  PERH  |= (SW5_MASK | SW2_MASK);     // Enable pull devices
  PPSH  |= (SW5_MASK | SW2_MASK);     // Pull-up mode

  // === Timer Setup ===
  
TSCR1 = 0x80;      // Enable timer
TSCR2 = 0x07;      // Prescaler /128
TIOS &= ~0x01;     // CH0 = input capture
TCTL4 |= 0x02;     // Falling edge
TFLG1 = 0x01;      // Clear old flag
TIOS=TIOS & ~0x01; //selects channel0 as input capture

  // === SCI Setup (9600 baud @ 24MHz) ===
  SCI0BDH = 0;
  SCI0BDL = 156;
  SCI0CR1 = 0x00;
  SCI0CR2 = 0x0C;

  while (1) {
    SCI_OutString("Ready:\r\n");
    start();
    stop();
    calc();
    done();
  }
}

void SerCom0(unsigned char data) {
  while (!(SCI0SR1 & 0x80));
  SCI0DRL = data;
}

void SCI_OutString(char *pt) {
  while (*pt) {
    while (!(SCI0SR1 & 0x80));
    SCI0DRL = *pt++;
  }
}

void MSDelay(unsigned int t){
unsigned int i, j;
for (i=0;i<t;i++)  
for(j=0;j<4000;j++) ;

}

void RandomDelay(void) {
  unsigned int delayMs = (rand() % 2000) + 1000;  // 1000-2999 ms
  MSDelay(delayMs);
}


// === Buzzer Control ===
void buzzer_on(void) {

  DDRT |= 0x20;         // PT5 output
  TIOS |= 0x20;         // Channel 5 = Output Compare
  TCTL1 |= 0x04;        // Toggle PT5 on match (OC5)
  TC5 = TCNT + 187;     // First match in 1 ms

while(PTT & 0x01){
while(!(TFLG1 & 0x20));
   
  TC5 += 187;     // First match in 1 ms
  TFLG1 = 0x20;
}
}
void buzzer_off(void) {
  TIOS &= ~0x20;      // Disable OC5
  TCTL1 &= ~0x0C;     // Disable toggle
  PTP &= ~BUZZER_MASK; // Drive buzzer pin low
}

// --- START FUNCTION ---
void start() {
  // Wait for SW5 press (PH0 goes LOW)
  while (PTH & SW5_MASK);
  MSDelay(20);             // debounce delay
  
  RandomDelay(); 

  T1 = TCNT;

  PORTB |= LED0_MASK;        // LED ON
  PTP |= 0x10;  // Buzzer ON (PTP4)
  buzzer_on();                // Buzzer ON
}

// --- STOP FUNCTION ---
void stop() {
  
  while ((PTT & 0x01) == 0);  // Wait for PT0 to be HIGH (button released)
  MSDelay(20);                // Debounce: wait for stable state

  while (!(TFLG1 & 0x01));    // Wait for falling edge on PT0
  Tstop = TC0;                // Read captured timer value
  TFLG1 = 0x01;               // Clear flag again

  PORTB &= ~LED0_MASK;        // Turn LED OFF
  PTP &= ~0x10;               // Turn Buzzer OFF
  buzzer_off();               // buzzer off
}

// --- TIME CALCULATION ---
void calc() {
  unsigned int ticks;

  if (Tstop < T1) {
    ticks = (0xFFFF - T1) + Tstop + 1;
  } else {
    ticks = Tstop - T1;
  }

  result = ticks / 187;

  r100 = (result / 100) % 10;
  r10  = (result / 10) % 10;
  r1   = result % 10;
}

// --- DISPLAY RESULT ---
void done() {
  SCI_OutString("YOUR TIME REACTION IS: ");
  SerCom0(r100 + '0');
  SerCom0(r10 + '0');
  SerCom0(r1 + '0');
  SCI_OutString(" ms\r\n");
}