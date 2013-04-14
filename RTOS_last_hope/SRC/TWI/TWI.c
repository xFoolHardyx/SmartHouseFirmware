/* Scheduler include files. */
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>


/* Application includes. */
#include <TWI.h>
#include <lib_AT91SAM7S256.h>

/* Misc constants. */
#define TWINO_BLOCK				( ( portTickType ) 0 )
#define TWIQUEUE_LENGTH			( ( unsigned char ) 5 )
#define TWIEXTRA_MESSAGES		( ( unsigned char ) 2 )
#define TWIREAD_TX_LEN			( ( unsigned long ) 2 )
#define TWIACTIVE_MASTER_MODE	( ( unsigned char ) 0x40 )

#define AT91C_TWI_CLOCK 	8000

#define twiINTERRUPT_LEVEL ((unsigned int) 0x4)

#define DisInt (AT91C_TWI_TXCOMP|AT91C_TWI_RXRDY|AT91C_TWI_TXRDY|AT91C_TWI_NACK)
#define TWISTA_BIT (AT91C_TWI_START|AT91C_TWI_MSEN)



// Array of message definitions.
static xTWIMessage xTxMessages[ TWIQUEUE_LENGTH + TWIEXTRA_MESSAGES ];

/* Function in the ARM part of the code used to create the queues. */

/* Index to the next free message in the xTxMessages array. */
unsigned long ulNextFreeMessage = ( unsigned long ) 0;

/* Queue of messages that are waiting transmission. */
static xQueueHandle xMessagesForTx;


/* Flag to indicate the state of the I2C ISR state machine. */
static unsigned long *pulBusFree;

/*-----------------------------------------------------------*/
void TWIMessage( const unsigned char * const pucMessage, long lMessageLength, unsigned char ucSlaveAddress, unsigned short usBufferAddress, unsigned long ulDirection, xSemaphoreHandle xMessageCompleteSemaphore, portTickType xBlockTime )
{
extern volatile xTWIMessage *pxCurrentMessage;
xTWIMessage *pxNextFreeMessage;
signed portBASE_TYPE xReturn;


	portENTER_CRITICAL();
	{

		pxNextFreeMessage = &( xTxMessages[ ulNextFreeMessage ] );

		/* Fill the message with the data to be sent. */


		pxNextFreeMessage->pucBuffer = ( unsigned char * ) pucMessage;

		pxNextFreeMessage->ucSlaveAddress = ucSlaveAddress | ( unsigned char ) ulDirection;

		pxNextFreeMessage->xMessageCompleteSemaphore = xMessageCompleteSemaphore;

		pxNextFreeMessage->lMessageLength = lMessageLength;

		pxNextFreeMessage->ucBufferAddress = ( unsigned char ) ( usBufferAddress & 0xff );

		/* Increment to the next message in the array - with a wrap around check. */
		ulNextFreeMessage++;

		if( ulNextFreeMessage >= ( TWIQUEUE_LENGTH + TWIEXTRA_MESSAGES ) )
		{
			ulNextFreeMessage = ( unsigned long ) 0;
		}

	}
	portEXIT_CRITICAL();
}

/*-----------------------------------------------------------*/

void AT91F_SetTwiClock(void)
{
//	unsigned int  sclock;
//	unsigned int cmp_sclock;
//
//		/* Here, CKDIV = 1 and CHDIV=CLDIV  ==> CLDIV = CHDIV = 1/4*((Fmclk/FTWI) -6)*/
//
//		sclock = (10*configCPU_CLOCK_HZ /AT91C_TWI_CLOCK);
//		if (sclock % 10 >= 5)
//			sclock = (sclock /10) - 5;
//		else
//			sclock = (sclock /10)- 6;
//		sclock = (sclock + (4 - sclock %4)) >> 2;	// div 4

	    AT91C_BASE_TWI->TWI_CWGR	= 0x1EEEE;
//	    0x1EEEE -  50000hz
//	    		( 1<<16 ) | (sclock << 8) | (sclock)  ;
//	    cmp_sclock=0;
//	    cmp_sclock=AT91C_BASE_TWI->TWI_CWGR;
}

void vTWICreateQueues( unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxTxMessages, unsigned long **ppulBusFree )
{
	/* Create the queues used to hold Rx and Tx characters. */
	xMessagesForTx = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( xTWIMessage * ) );

	/* Pass back a reference to the queue and bus free flag so the I2C API file
	can post messages. */
	*pxTxMessages = xMessagesForTx;
	*ppulBusFree = &ulBusFree;
}

void TWIInit( void )
{
extern void ( vTWI_ISR_Wrapper )( void );

AT91PS_AIC	pAIC = AT91C_BASE_AIC;

	/* Create the queue used to send messages to the ISR. */
	vTWICreateQueues( TWIQUEUE_LENGTH, &xMessagesForTx, &pulBusFree );

	/* Configure the I2C hardware. */

	// Configure TWI PIOs
	AT91F_TWI_CfgPIO ();
	// Configure PMC by enabling TWI clock
	AT91F_TWI_CfgPMC ();

	// Configure TWI in master mode
	AT91F_TWI_Configure ();

	// Set TWI Clock Waveform Generator Register
	AT91F_SetTwiClock();

	portENTER_CRITICAL();
	{
//		AT91F_AIC_ConfigureIt(AT91C_ID_TWI,twiINTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_EXT_POSITIVE_EDGE,( void (*)(void) )vTWI_ISR_Wrapper);
		pAIC->AIC_IDCR = (1<<AT91C_ID_TWI);					// Disable TWI interrupt in AIC Interrupt Disable Command Register
		pAIC->AIC_SVR[AT91C_ID_TWI] =						// Set the TWI IRQ handler address in AIC Source
		(unsigned int)vTWI_ISR_Wrapper;        			// Vector Register[9]
		pAIC->AIC_SMR[AT91C_ID_TWI] =						// Set the interrupt source type(level-sensitive) and
		(AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE | 0x4 ); 		// priority (4)in AIC Source Mode Register[6]
		pAIC->AIC_ICCR = (1<<AT91C_ID_TWI);					// Clear the interrupt on the interrupt controller
		pAIC->AIC_IECR = (1 << AT91C_ID_TWI); 				// Enable the TWI interrupt in AIC Interrupt Enable Command Register

	}
	portEXIT_CRITICAL();
}

