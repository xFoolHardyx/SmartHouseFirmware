//  *****************************************************************************
//    usart0_isr.c
//
// USART0 Interrupt Service Routine
//
// This demonstration is designed to read 10 characters into a buffer.
// After the 10th character arrives, transmit the 10 characters back.
//
// The application is interrupt-driven.
//
//  Author:  James P Lynch  June 22, 2008
//  *****************************************************************************
//  *******************************************************
//                Header Files
//  *******************************************************
#include "include/AT91SAM7S128.h"
#include "include/board.h"
//  *******************************************************
//                Global Variables
//  *******************************************************
char Buffer[32]; // holds received characters
unsigned long nChars = 0; // counts number of received chars
char *pBuffer = &Buffer[0]; // pointer into Buffer
void Usart0IrqHandler(void) {

	volatile AT91PS_USART pUsart0 = AT91C_BASE_US0; // create a pointer to USART0 structure
// determine which interrupt has occurred
// assume half-duplex operation here, only one interrupt type at a time
	if ((pUsart0->US_CSR & AT91C_US_RXRDY) == AT91C_US_RXRDY) {
// we have a receive interrupt,
// remove it from Receiver Holding Register and place into buffer[]
		*pBuffer++ = pUsart0->US_RHR;
		nChars++;
// check if 10 characters have been received
		if (nChars >= 10) {
// yes, redirect buffer pointer to beginning
			pBuffer = &Buffer[0];
			nChars = 0;
// disable the receive interrupt, enable the transmit interrupt
			pUsart0->US_IER = AT91C_US_TXEMPTY; // enable TXEMPTY usart0 transmit interrupt
			pUsart0->US_IDR = ~AT91C_US_TXEMPTY; // disable all interrupts except TXEMPTY
// send first received character, TXEMPTY interrupt will send the rest
			pUsart0->US_THR = *pBuffer++;
			nChars++;
		}
	} else if ((pUsart0->US_CSR & AT91C_US_TXEMPTY) == AT91C_US_TXEMPTY) {
// we have a transmit interrupt (previous char has clocked out)
// check if 10 characters have been transmitted
		if (nChars >= 10) {
// yes, redirect buffer pointer to beginning
			pBuffer = &Buffer[0];
			nChars = 0;
// enable receive interrupt, disable the transmit interrupt
			pUsart0->US_IER = AT91C_US_RXRDY; // enable RXRDY usart0 receive interrupt
			pUsart0->US_IDR = ~AT91C_US_RXRDY; // disable all interrupts except RXRDY
		} else {
// no, send next character
			pUsart0->US_THR = *pBuffer++;
			nChars++;
		}
	}
}
