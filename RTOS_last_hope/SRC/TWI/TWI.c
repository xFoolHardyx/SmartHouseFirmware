/* Scheduler include files. */
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>


/* Application includes. */
#include <TWI.h>
#include <lib_AT91SAM7S256.h>

/*-----------------------------------------------------------*/

/* Misc constants. */
#define TWINO_BLOCK				( ( portTickType ) 0 )
#define TWIQUEUE_LENGTH			( ( unsigned char ) 5 )
#define TWIEXTRA_MESSAGES		( ( unsigned char ) 2 )
#define TWIREAD_TX_LEN			( ( unsigned long ) 2 )
#define TWIACTIVE_MASTER_MODE	( ( unsigned char ) 0x40 )
#define TWITIMERL				( 200 )
#define TWITIMERH				( 200 )

#define AT91C_TWI_CLOCK 	8000

#define twiINTERRUPT_LEVEL ((unsigned int) 0x5)

#define DisInt (AT91C_TWI_TXCOMP|AT91C_TWI_RXRDY|AT91C_TWI_TXRDY|AT91C_TWI_NACK)
#define TWISTA_BIT (AT91C_TWI_START|AT91C_TWI_MSEN)



// Array of message definitions.
static xTWIMessage xTxMessages[ TWIQUEUE_LENGTH + TWIEXTRA_MESSAGES ];

/* Function in the ARM part of the code used to create the queues. */
//extern void vI2CISRCreateQueues( unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxTxMessages, unsigned long **ppulBusFree );

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
		/* This message is guaranteed to be free as there are two more messages
		than spaces in the queue allowing for one message to be in process of
		being transmitted and one to be left free. */
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

		/* Is the I2C interrupt in the middle of transmitting a message? */
		if( *pulBusFree == ( unsigned long ) pdTRUE )
		{
			/* No message is currently being sent or queued to be sent.  We
			can start the ISR sending this message immediately. */
			pxCurrentMessage = pxNextFreeMessage;

//			I2C_I2CONCLR = i2cSI_BIT;
//			I2C_I2CONSET = i2cSTA_BIT;
			AT91C_BASE_TWI->TWI_IDR=DisInt;
			AT91C_BASE_TWI->TWI_CR=TWISTA_BIT;

			*pulBusFree = ( unsigned long ) pdFALSE;
		}
		else
		{
			/* The I2C interrupt routine is mid sending a message.  Queue
			this message ready to be sent. */
			xReturn = xQueueSend( xMessagesForTx, &pxNextFreeMessage, xBlockTime );

			/* We may have blocked while trying to queue the message.  If this
			was the case then the interrupt would have been enabled and we may
			now find that the I2C interrupt routine is no longer sending a
			message. */
			if( ( *pulBusFree == ( unsigned long ) pdTRUE ) && ( xReturn == pdPASS ) )
			{
				/* Get the next message in the queue (this should be the
				message we just posted) and start off the transmission
				again. */
				xQueueReceive( xMessagesForTx, &pxNextFreeMessage, TWINO_BLOCK );
				pxCurrentMessage = pxNextFreeMessage;

//				I2C_I2CONCLR = i2cSI_BIT;
//				I2C_I2CONSET = i2cSTA_BIT;
				//AT91C_BASE_TWI->TWI_IDR=DisInt;
				AT91C_BASE_TWI->TWI_CR=TWISTA_BIT;

				*pulBusFree = ( unsigned long ) pdFALSE;
			}
		}
	}
	portEXIT_CRITICAL();
}

/*-----------------------------------------------------------*/

void AT91F_SetTwiClock(void)
{
	unsigned int  sclock;
	unsigned int cmp_sclock;

		/* Here, CKDIV = 1 and CHDIV=CLDIV  ==> CLDIV = CHDIV = 1/4*((Fmclk/FTWI) -6)*/

		sclock = (10*configCPU_CLOCK_HZ /AT91C_TWI_CLOCK);
		if (sclock % 10 >= 5)
			sclock = (sclock /10) - 5;
		else
			sclock = (sclock /10)- 6;
		sclock = (sclock + (4 - sclock %4)) >> 2;	// div 4

	    AT91C_BASE_TWI->TWI_CWGR	= ( 1<<16 ) | (sclock << 8) | (sclock)  ;
	    cmp_sclock=0;
	    cmp_sclock=AT91C_BASE_TWI->TWI_CWGR;
}

void TWIInit( void )
{
extern void ( vTWI_ISR_Wrapper )( void );

	/* Create the queue used to send messages to the ISR. */
	vTWIISRCreateQueues( TWIQUEUE_LENGTH, &xMessagesForTx, &pulBusFree );

	/* Configure the I2C hardware. */

//	I2C_I2CONCLR = 0xff;
//
//	PCB_PINSEL0 |= mainSDA_ENABLE;
//	PCB_PINSEL0 |= mainSCL_ENABLE;
//
//	I2C_I2SCLL = i2cTIMERL;
//	I2C_I2SCLH = i2cTIMERH;
//	I2C_I2CONSET = i2cACTIVE_MASTER_MODE;

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
		/* Setup the VIC for the i2c interrupt. */
//		VICIntSelect &= ~( i2cI2C_VIC_CHANNEL_BIT );
//		VICIntEnable |= i2cI2C_VIC_CHANNEL_BIT;
//		VICVectAddr2 = ( long ) vI2C_ISR_Wrapper;
//
//		VICVectCntl2 = i2cI2C_VIC_CHANNEL | i2cI2C_VIC_ENABLE;

		AT91F_AIC_ConfigureIt(AT91C_ID_TWI,twiINTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_EXT_POSITIVE_EDGE,( void (*)(void) )vTWI_ISR_Wrapper);
		AT91C_BASE_AIC->AIC_IECR = 0x1 << AT91C_ID_TWI ;

	}
	portEXIT_CRITICAL();
}

