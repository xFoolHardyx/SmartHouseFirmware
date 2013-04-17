/* Scheduler include files. */
#include <FreeRTOS.h>
#include <queue.h>


/* Application includes. */
#include <TWI.h>
#include <lib_AT91SAM7S256.h>

//#include <TWI_ISR_MY.h>

/* Misc constants. */
#define TWINO_BLOCK				( ( portTickType ) 0 )
#define TWIQUEUE_LENGTH			( ( unsigned char ) 10 )
#define TWIEXTRA_MESSAGES		( ( unsigned char ) 2 )
#define TWIREAD_TX_LEN			( ( unsigned long ) 2 )
#define TWIACTIVE_MASTER_MODE	( ( unsigned char ) 0x40 )

#define AT91C_TWI_CLOCK 	8000

#define twiINTERRUPT_LEVEL ((unsigned int) 0x4)

#define uxQueueLength ((unsigned long) 10)

static xTWIMessage xTxMessages[2];



/* Queue of messages that are waiting transmission. */
xQueueHandle xMessagesForTx;

/*-----------------------------------------------------------*/
void TWIMessage( unsigned char * pucMessage, long lMessageLength, unsigned char ucSlaveAddress, unsigned int ucCountIntAddr, unsigned uBufferAddress, unsigned char ucDirection, unsigned long ulTicksToWait)
{
 xTWIMessage * pxNextFreeMessage;


 pxNextFreeMessage = &( xTxMessages[ 0 ] );


	portENTER_CRITICAL();
	{
		/* Fill the message with the data to be sent. */
		pxNextFreeMessage->pucBuf =  pucMessage; // buf

		pxNextFreeMessage->ucDevAddr = ucSlaveAddress; 				// dev addr

		pxNextFreeMessage->lMessageLength = lMessageLength;			// message length

		pxNextFreeMessage->uInternalAddr = uBufferAddress;			// internal register addr

		pxNextFreeMessage->ucDirection = ucDirection;				// Read or Write

		pxNextFreeMessage->ucInternalSizeAddr = ucCountIntAddr;		// Select internal addr byte to send


		xQueueSend(xMessagesForTx, &pxNextFreeMessage, ulTicksToWait);
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

void TWIInit( void )
{
extern void ( vTWI_ISR_Wrapper )( void );

	AT91PS_AIC	pAIC = AT91C_BASE_AIC;


	xMessagesForTx = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( xTWIMessage  ) );

	/* Configure the TWI hardware. */

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

