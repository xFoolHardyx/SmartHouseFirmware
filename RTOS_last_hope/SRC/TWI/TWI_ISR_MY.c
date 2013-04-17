#include <AT91SAM7X256.h>
#include <TWI_ISR_MY.h>
#include <FreeRTOS.h>
#include <TWI.h>


//xReadyMessage  xFlags;
unsigned char TWI_TX_Ready = 0;
unsigned char TWI_TX_Comp = 0;
unsigned char TWI_RX_Ready = 0;

/* The ISR entry point. */
void vTWI_ISR_Wrapper( void ) __attribute__ (( naked ));

/* The ISR function to perform the actual work.  This must be a separate
function from the wrapper to ensure the correct stack frame is set up. */
void vTWI_ISR_Handler( void );

/*-----------------------------------------------------------*/

void vTWI_ISR_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* Call the handler to perform the actual work.  This must be a
	separate function to ensure the correct stack frame is set up. */
	vTWI_ISR_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

void vTWI_ISR_Handler( void )
{
//	unsigned int uiStatus;
	unsigned int uiMaskInterrupt;


	volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;	// create a pointer to TWI structure

	uiMaskInterrupt = pTWI->TWI_IMR;			//Interrupt Mask Register

	if(uiMaskInterrupt & AT91C_TWI_TXRDY)
	{
		pTWI->TWI_IDR = AT91C_TWI_TXRDY; 		// disable interrupt
		TWI_RX_Ready = TWI_TRUE;
		uiMaskInterrupt = pTWI->TWI_IMR;
		return;
	}
	else if(uiMaskInterrupt & AT91C_TWI_TXCOMP) // Transmission complete
	{
		pTWI->TWI_IDR = AT91C_TWI_TXCOMP; 		// disable interrupt
		TWI_TX_Comp = TWI_TRUE;
	}
	else if(uiMaskInterrupt & AT91C_TWI_RXRDY) 	// Received data
	{
		pTWI->TWI_IDR = AT91C_TWI_RXRDY; 		// disable interrupt
		TWI_TX_Ready = TWI_TRUE;
	}

//  uiStatus = pTWI->TWI_SR;					// Status Register
//
//		if(uiStatus & AT91C_TWI_NACK)
//		{
//			TWI_Ack = AT91C_TWI_NACK;
//		}
//		else
//		{
//			TWI_Ack = AT91C_TWI_ACK;
//		}


}
