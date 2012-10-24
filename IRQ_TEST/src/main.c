// ****************************************************************************************************************
//    main.c
//
// Interrupt-driven USART0 demonstration program for Olimex SAM7-EX256 Evaluation Board
//
// This simple demo reads 10 characters from USART0 (9600 baud, 8 data bits, 1 stop bit, no parity)
// When 10 characters are read, they are transmitted back to the source.
//
// Use standard RS-232 serial cable and the Windows HyperTerm program to test.
//
// Blinks LED (pin PA3) with an endless loop
// PA3 is pin 1 on the EXT 20-pin connector (3.3v is pin 18)
//
// The Olimex SAM7-EX256 board has no programmable LEDs.
// Added a simple test LED from Radio Shack as shown below (can be attached to the 20-pin EXT connector.)
//
// 3.3 volts  |---------|           anode  |----|                PA3
// EXT O------| 470 ohm |------------------|LED |--------------O EXT
// Pin 18     |---------|                  |----| cathode        pin 1
//                                              Radio Shack Red LED
//                                              276-026 T-1 size (anode is the longer wire)
//
//          LED current:  I = E/R  = 3.3/470  =  .007 amps = 7 ma
// Note: most PIO pins can drive 8 ma on the AT91SAM7X256, so we're OK
//
//
//
// Author:  James P Lynch  June 22, 2008
// ****************************************************************************************************************

//  *******************************************************
//                Header Files
//  *******************************************************
#include "include/AT91SAM7S128.h"
#include "include/board.h"
//  *******************************************************
//                External References
//  *******************************************************
extern void LowLevelInit(void);
extern void USART0Setup(void);
extern unsigned enableIRQ(void);
int main (void) {
unsigned long j;
unsigned int IdleCount = 0;
// Initialize the Atmel AT91SAM7S256 (watchdog, PLL clock, default interrupts, etc.)
LowLevelInit();
// Set up the LED (PA3)
volatile AT91PS_PIO pPIO = AT91C_BASE_PIOA; // pointer to PIO data structure
pPIO->PIO_PER = LED_MASK; // PIO Enable Register - allow PIO to control pin PP3
pPIO->PIO_OER = LED_MASK; // PIO Output Enable Register - sets pin P3 to outputs
pPIO->PIO_SODR = LED_MASK; // PIO Set Output Data Register - turns off the LED
// set up USART0
USART0Setup();
// enable global interrupts
enableIRQ();
// *****************************
// *  endless blink loop       *
// *****************************
while (1)
{
	if  ((pPIO->PIO_ODSR & LED4) == LED4) // read previous state of LED4
		pPIO->PIO_CODR = LED4; // turn LED4 (DS1) on
	else
		pPIO->PIO_SODR = LED4; // turn LED4 (DS1) off

	for (j = 1000000; j != 0; j-- ); // wait 1 second 1000000
	IdleCount++; // count # of times through the idle loop
}
}
