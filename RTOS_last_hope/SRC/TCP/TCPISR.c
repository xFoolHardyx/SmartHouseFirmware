/* Scheduler include files. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>


#include <AT91SAM7X256.h>



/* Constants required for interrupt management. */
#define tcpCLEAR_VIC_INTERRUPT	( 0 )
#define tcpEINT0_VIC_CHANNEL_BIT	( ( unsigned long ) 0x4000 )

#define AT91C_ID_IRQ0   ((unsigned int) 30)
#define AT91C_BASE_AIC       ((AT91PS_AIC) 	0xFFFFF000) // (AIC) Base Address


/* EINT0 interrupt handler.  This processes interrupts from the WIZnet device. */
void vEINT0_ISR_Wrapper( void ) __attribute__((naked));

/* The handler that goes with the EINT0 wrapper. */
void vEINT0_ISR_Handler( void );

/* Variable is required for its address, but does not otherwise get used. */
static long lDummyVariable;

/*
 * When the WIZnet device asserts an interrupt we send an (empty) message to
 * the TCP task.  This wakes the task so the interrupt can be processed.  The
 * source of the interrupt has to be ascertained by the TCP task as this
 * requires an I2C transaction which cannot be performed from this ISR.
 * Note this code predates the introduction of semaphores, a semaphore should
 * be used in place of the empty queue message.
 */
void vEINT0_ISR_Handler( void )
{
extern xQueueHandle xTCPISRQueue;
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Just wake the TCP task so it knows an ISR has occurred. */
	xQueueSendFromISR( xTCPISRQueue, ( void * ) &lDummyVariable, &xHigherPriorityTaskWoken );

	/* We cannot carry on processing interrupts until the TCP task has
	processed this one - so for now interrupts are disabled.  The TCP task will
	re-enable it. */

	//VICIntEnClear |= tcpEINT0_VIC_CHANNEL_BIT;

	AT91C_BASE_AIC->AIC_IDCR = 1<<AT91C_ID_IRQ0 ;
	    //* Clear the interrupt on the Interrupt Controller ( if one is pending )
	AT91C_BASE_AIC->AIC_ICCR = 1<<AT91C_ID_IRQ0 ;


	/* Clear the interrupt bit. */
	//VICVectAddr = tcpCLEAR_VIC_INTERRUPT;

	if( xHigherPriorityTaskWoken )
	{
		portYIELD_FROM_ISR();
	}
}
/*-----------------------------------------------------------*/

void vEINT0_ISR_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* The handler must be a separate function from the wrapper to
	ensure the correct stack frame is set up. */
	vEINT0_ISR_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}


