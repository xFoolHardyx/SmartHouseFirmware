/* Scheduler include files. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
/* Application includes. */
//#include <TWI.h>

#define AT91C_TWI_ACK 0

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
	unsigned int Status_Reg, Mask_Reg;
	volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;		// create a pointer to TWI structure

	Mask_Reg = pTWI->TWI_IMR;	//Interrupt Mask Register

	if(Mask_Reg & AT91C_TWI_TXRDY)
	{
		pTWI->TWI_IDR = AT91C_TWI_TXRDY; // disable interrupt
		TWI_TX_Ready = 1;
		return;
	}
	else if(Mask_Reg & AT91C_TWI_TXCOMP) // Transmission complete
	{
		pTWI->TWI_IDR = AT91C_TWI_TXCOMP; // disable interrupt
		TWI_TX_Comp = 1;
	}
	else if(Mask_Reg & AT91C_TWI_RXRDY) // Received data
	{
		pTWI->TWI_IDR = AT91C_TWI_RXRDY; // disable interrupt
		TWI_RX_Ready = 1;
	}

	Status_Reg = pTWI->TWI_SR;	// Status Register
	if(Status_Reg & AT91C_TWI_NACK)
	{
		TWI_Ack = AT91C_TWI_NACK;
	}
	else
	{
		TWI_Ack = AT91C_TWI_ACK;
	}


///* Holds the current transmission state. */


//portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
//long lBytesLeft;
//
//	if( xHigherPriorityTaskWoken )
//	{
//		portYIELD_FROM_ISR(); // forcibly [принудительно] switch context
//	}
}
/*-----------------------------------------------------------*/

