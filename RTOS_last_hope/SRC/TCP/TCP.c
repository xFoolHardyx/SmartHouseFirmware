#include <TCP.h>
#include <FreeRTOS.h>
#include <settings.h>

/*-----------------------------------------------------------*/

/* Hardwired i2c address of the WIZNet device. */
#define tcpDEVICE_ADDRESS 				( ( unsigned char ) 0x00 )

/* Constants used to configure the Tx and Rx buffer sizes within the WIZnet
device. */
#define tcp8K_RX						( ( unsigned char ) 0x03 )
#define tcp8K_TX						( ( unsigned char ) 0x03 )

/* Constants used to generate the WIZnet internal buffer addresses. */
#define tcpSINGLE_SOCKET_ADDR_MASK		( ( unsigned long ) 0x1fff )
#define tcpSINGLE_SOCKET_ADDR_OFFSET	( ( unsigned long ) 0x4000 )

/* Bit definitions of the commands that can be sent to the command register. */
#define tcpRESET_CMD					( ( unsigned char ) 0x80 )
#define tcpSYS_INIT_CMD					( ( unsigned char ) 0x01 )
#define tcpSOCK_STREAM					( ( unsigned char ) 0x01 )
#define tcpSOCK_INIT					( ( unsigned char ) 0x02 )
#define tcpLISTEN_CMD					( ( unsigned char ) 0x08 )
#define tcpRECEIVE_CMD					( ( unsigned char ) 0x40 )
#define tcpDISCONNECT_CMD				( ( unsigned char ) 0x10 )
#define tcpSEND_CMD						( ( unsigned char ) 0x20 )

/* Constants required to handle the interrupts. */
#define tcpCLEAR_EINT0					( 1 )
#define i2cCLEAR_ALL_INTERRUPTS			( ( unsigned char ) 0xff )
#define i2cCHANNEL_0_ISR_ENABLE			( ( unsigned char ) 0x01 )
#define i2cCHANNEL_0_ISR_DISABLE		( ( unsigned char ) 0x00 )
#define tcpWAKE_ON_EINT0				( 1 )
#define tcpENABLE_EINT0_FUNCTION		( ( unsigned long ) 0x01 )
#define tcpEINT0_VIC_CHANNEL_BIT		( ( unsigned long ) 0x4000 )
#define tcpEINT0_VIC_CHANNEL			( ( unsigned long ) 14 )
#define tcpEINT0_VIC_ENABLE				( ( unsigned long ) 0x0020 )

/* Various delays used in the driver. */
#define tcpRESET_DELAY					( ( portTickType ) 16 / portTICK_RATE_MS )
#define tcpINIT_DELAY					( ( portTickType ) 500 / portTICK_RATE_MS  )
#define tcpLONG_DELAY					( ( portTickType ) 500 / portTICK_RATE_MS  )
#define tcpSHORT_DELAY					( ( portTickType ) 5 / portTICK_RATE_MS )
#define tcpCONNECTION_WAIT_DELAY		( ( portTickType ) 100 / portTICK_RATE_MS )
#define tcpNO_DELAY						( ( portTickType ) 0 )

/* Length of the data to read for various register reads. */
#define tcpSTATUS_READ_LEN				( ( unsigned long ) 1 )
#define tcpSHADOW_READ_LEN				( ( unsigned long ) 1 )

/* Register addresses within the WIZnet device. */
#define tcpCOMMAND_REG					( ( unsigned short ) 0x0000 )
#define tcpGATEWAY_ADDR_REG				( ( unsigned short ) 0x0080 )
#define tcpSUBNET_MASK_REG				( ( unsigned short ) 0x0084 )
#define tcpSOURCE_HA_REG				( ( unsigned short ) 0x0088 )
#define tpcSOURCE_IP_REG				( ( unsigned short ) 0x008E )
#define tpcSOCKET_OPT_REG				( ( unsigned short ) 0x00A1 )
#define tcpSOURCE_PORT_REG				( ( unsigned short ) 0x00AE )
#define tcpTX_WRITE_POINTER_REG			( ( unsigned short ) 0x0040 )
#define tcpTX_READ_POINTER_REG			( ( unsigned short ) 0x0044 )
#define tcpTX_ACK_POINTER_REG			( ( unsigned short ) 0x0018 )
#define tcpTX_MEM_SIZE_REG				( ( unsigned short ) 0x0096 )
#define tcpRX_MEM_SIZE_REG				( ( unsigned short ) 0x0095 )
#define tcpINTERRUPT_STATUS_REG			( ( unsigned short ) 0x0004 )
#define tcpTX_WRITE_SHADOW_REG			( ( unsigned short ) 0x01F0 )
#define tcpTX_ACK_SHADOW_REG			( ( unsigned short ) 0x01E2 )
#define tcpISR_MASK_REG					( ( unsigned short ) 0x0009 )
#define tcpINTERRUPT_REG				( ( unsigned short ) 0x0008 )
#define tcpSOCKET_STATE_REG				( ( unsigned short ) 0x00a0 )

/* Constants required for hardware setup. */
#define tcpRESET_ACTIVE_LOW 			( ( unsigned long ) 0x20 )
#define tcpRESET_ACTIVE_HIGH 			( ( unsigned long ) 0x10 )

/* Constants defining the source of the WIZnet ISR. */
#define tcpISR_SYS_INIT					( ( unsigned char ) 0x01 )
#define tcpISR_SOCKET_INIT				( ( unsigned char ) 0x02 )
#define tcpISR_ESTABLISHED				( ( unsigned char ) 0x04 )
#define tcpISR_CLOSED					( ( unsigned char ) 0x08 )
#define tcpISR_TIMEOUT					( ( unsigned char ) 0x10 )
#define tcpISR_TX_COMPLETE				( ( unsigned char ) 0x20 )
#define tcpISR_RX_COMPLETE				( ( unsigned char ) 0x40 )

/* Constants defining the socket status bits. */
#define tcpSTATUS_ESTABLISHED			( ( unsigned char ) 0x06 )
#define tcpSTATUS_LISTEN				( ( unsigned char ) 0x02 )

/* Misc constants. */
#define tcpNO_STATUS_BITS				( ( unsigned char ) 0x00 )
#define i2cNO_ADDR_REQUIRED				( ( unsigned short ) 0x0000 )
#define i2cNO_DATA_REQUIRED				( 0x0000 )
#define tcpISR_QUEUE_LENGTH				( ( unsigned portBASE_TYPE ) 10 )
#define tcpISR_QUEUE_ITEM_SIZE			( ( unsigned portBASE_TYPE ) 0 )
#define tcpBUFFER_LEN					( 4 * 1024 )
#define tcpMAX_REGISTER_LEN				( 4 )
#define tcpMAX_ATTEMPTS_TO_CHECK_BUFFER	( 6 )
#define tcpMAX_NON_LISTEN_STAUS_READS	( 5 )

/* Message definitions.  The IP address, MAC address, gateway address, etc.
is set here! */
unsigned char ucDataGAR[]				= { 172, 25, 218, 3 };	/* Gateway address. */
unsigned char ucDataMSR[]				= { 255, 255, 255, 0 };	/* Subnet mask.		*/
unsigned char ucDataSIPR[]				= { 172, 25, 218, 201 };/* IP address.		*/
unsigned char ucDataSHAR[]				= { 00, 23, 30, 41, 15, 26 }; /* MAC address - DO NOT USE THIS ON A PUBLIC NETWORK! */

/* Other fixed messages. */
unsigned char ucDataReset[]				= { tcpRESET_CMD };
unsigned char ucDataInit[]				= { tcpSYS_INIT_CMD };
unsigned char ucDataProtocol[]			= { tcpSOCK_STREAM };
unsigned char ucDataPort[]				= { 0xBA, 0xCC };
unsigned char ucDataSockInit[]			= { tcpSOCK_INIT };
unsigned char ucDataTxWritePointer[]	= { 0x11, 0x22, 0x00, 0x00 };
unsigned char ucDataTxAckPointer[]		= { 0x11, 0x22, 0x00, 0x00 };
unsigned char ucDataTxReadPointer[]		= { 0x11, 0x22, 0x00, 0x00 };
unsigned char ucDataListen[]			= { tcpLISTEN_CMD };
unsigned char ucDataReceiveCmd[]		= { tcpRECEIVE_CMD };
unsigned char ucDataSetTxBufSize[]		= { tcp8K_TX };
unsigned char ucDataSetRxBufSize[] 		= { tcp8K_RX };
unsigned char ucDataSend[]				= { tcpSEND_CMD };
unsigned char ucDataDisconnect[]		= { tcpDISCONNECT_CMD };
unsigned char ucDataEnableISR[]			= { i2cCHANNEL_0_ISR_ENABLE };
unsigned char ucDataDisableISR[]		= { i2cCHANNEL_0_ISR_DISABLE };
unsigned char ucDataClearInterrupt[]	= { i2cCLEAR_ALL_INTERRUPTS };

static void prvReadRegister( unsigned char *pucDestination, unsigned short usAddress, unsigned long ulLength )
{
unsigned char ucRxBuffer[ tcpMAX_REGISTER_LEN ];

	/* Read a register value from the WIZnet device. */

	/* First write out the address of the register we want to read. */
	vTWIMessage( ucRxBuffer, i2cNO_DATA_REQUIRED, tcpDEVICE_ADDRESS, usAddress, TWIWrite);

	/* Then read back from that address. */
	vTWIMessage( ( unsigned char * ) pucDestination, ulLength, tcpDEVICE_ADDRESS, i2cNO_ADDR_REQUIRED, TWIRead);
}

void vTCPHardReset( void )
{
	vTWIMessage(ucDataEnableISR, sizeof(i2cCHANNEL_0_ISR_ENABLE), tcpDEVICE_ADDRESS, tcpISR_MASK_REG, TWIWrite);
	vTaskDelay( tcpLONG_DELAY );
}

long lTCPSoftReset( void )
{
	unsigned char ucStatus;

	vTWIMessage(ucDataReset, sizeof(ucDataReset), tcpDEVICE_ADDRESS, tcpCOMMAND_REG, TWIWrite);
	vTWIMessage(ucDataSHAR, sizeof(ucDataSHAR), tcpDEVICE_ADDRESS, tcpSOURCE_HA_REG, TWIWrite);
	vTWIMessage(ucDataGAR, sizeof(ucDataGAR), tcpDEVICE_ADDRESS, tcpGATEWAY_ADDR_REG, TWIWrite);
	vTWIMessage(ucDataMSR, sizeof(ucDataMSR), tcpDEVICE_ADDRESS, tcpSUBNET_MASK_REG, TWIWrite);
	vTWIMessage(ucDataSIPR, sizeof(ucDataSIPR), tcpDEVICE_ADDRESS, tpcSOURCE_IP_REG, TWIWrite);
	vTWIMessage(ucDataSetTxBufSize, sizeof(ucDataSetTxBufSize), tcpDEVICE_ADDRESS, tcpTX_MEM_SIZE_REG, TWIWrite);
	vTWIMessage(ucDataSetRxBufSize, sizeof(ucDataSetRxBufSize), tcpDEVICE_ADDRESS, tcpRX_MEM_SIZE_REG, TWIWrite);
	vTWIMessage(ucDataInit, sizeof(ucDataInit), tcpDEVICE_ADDRESS, tcpCOMMAND_REG, TWIWrite);

	vTaskDelay( tcpINIT_DELAY );

	prvReadRegister( &ucStatus, tcpINTERRUPT_STATUS_REG, tcpSTATUS_READ_LEN );
}

long lTCPCreateSocket( void )
{

	unsigned char ucStatus;

	/* Create and configure a socket. */

	/* Setup and init the socket.  Here the port number is set and the socket
	is initialised. */
	vTWIMessage( ucDataProtocol, sizeof( ucDataProtocol),tcpDEVICE_ADDRESS, tpcSOCKET_OPT_REG, TWIWrite);
	vTWIMessage( ucDataPort,	sizeof( ucDataPort),	tcpDEVICE_ADDRESS, tcpSOURCE_PORT_REG, TWIWrite);
	vTWIMessage( ucDataSockInit, sizeof( ucDataSockInit),tcpDEVICE_ADDRESS, tcpCOMMAND_REG, TWIWrite);

		/* Allow the socket to initialise. */
		vTaskDelay( tcpINIT_DELAY );

		/* Read back the status to ensure the socket initialised ok. */
		prvReadRegister( &ucStatus, tcpINTERRUPT_STATUS_REG, tcpSTATUS_READ_LEN );

		/* Setup the Tx pointer registers to indicate that the Tx buffer is empty. */
		vTWIMessage( ucDataTxReadPointer, sizeof( ucDataTxReadPointer ), tcpDEVICE_ADDRESS, tcpTX_READ_POINTER_REG, TWIWrite);
		vTaskDelay( tcpSHORT_DELAY );
		vTWIMessage( ucDataTxWritePointer, sizeof( ucDataTxWritePointer ), tcpDEVICE_ADDRESS, tcpTX_WRITE_POINTER_REG, TWIWrite);
		vTaskDelay( tcpSHORT_DELAY );
		vTWIMessage( ucDataTxAckPointer,	  sizeof( ucDataTxAckPointer ),	  tcpDEVICE_ADDRESS, tcpTX_ACK_POINTER_REG, TWIWrite);
		vTaskDelay( tcpSHORT_DELAY );

		return ( long ) pdPASS;
}

long lProcessConnection( void )
{
	unsigned char ucISR, ucState, ucLastState = 2, ucShadow;
	extern volatile long lTransactionCompleted;
	long lSameStateCount = 0, lDataSent = pdFALSE;
	unsigned long ulWritePointer, ulAckPointer;

		/* No I2C errors can yet have occurred. */
		portENTER_CRITICAL();
			lTransactionCompleted = pdTRUE;
		portEXIT_CRITICAL();

		/* Keep looping - processing interrupts, until we have completed a
		transaction.   This uses the WIZnet in it's simplest form.  The socket
		accepts a connection - we process the connection - then close the socket.
		We then go back to reinitialise everything and start again. */
		while( lTransactionCompleted == pdTRUE )
		{
			/* Wait for a message on the queue from the WIZnet ISR.  When the
			WIZnet device asserts an interrupt the ISR simply posts a message
			onto this queue to wake this task. */
			if( xQueueReceive( xTCPISRQueue, &ucISR, tcpCONNECTION_WAIT_DELAY ) )
			{
				/* The ISR posted a message on this queue to tell us that the
				WIZnet device asserted an interrupt.  The ISR cannot process
				an I2C message so cannot tell us what caused the interrupt so
				we have to query the device here.  This task is the highest
				priority in the system so will run immediately following the ISR. */
				prvReadRegister( &ucISR, tcpINTERRUPT_STATUS_REG, tcpSTATUS_READ_LEN );

				/* Once we have read what caused the ISR we can clear the interrupt
				in the WIZnet. */
				i2cMessage( ucDataClearInterrupt, sizeof( ucDataClearInterrupt ), tcpDEVICE_ADDRESS, tcpINTERRUPT_REG, i2cWRITE, NULL, portMAX_DELAY );

				/* Now we can clear the processor interrupt and re-enable ready for
				the next. */
				SCB_EXTINT = tcpCLEAR_EINT0;
				VICIntEnable |= tcpEINT0_VIC_CHANNEL_BIT;

				/* Process the interrupt ... */

				if( ucISR & tcpISR_ESTABLISHED )
				{
					/* A connection has been established - respond by sending
					a receive command. */
					i2cMessage( ucDataReceiveCmd, sizeof( ucDataReceiveCmd ), tcpDEVICE_ADDRESS, tcpCOMMAND_REG, i2cWRITE, NULL, portMAX_DELAY );
				}

				if( ucISR & tcpISR_RX_COMPLETE )
				{
					/* We message has been received.  This will be an HTTP get
					command.  We only have one page to send so just send it without
					regard to what the actual requested page was. */
					prvSendSamplePage();
				}

			 	if( ucISR & tcpISR_TX_COMPLETE )
				{
					/* We have a TX complete interrupt - which oddly does not
					indicate that the message being sent is complete so we cannot
					yet close the socket.  Instead we read the position of the Tx
					pointer within the WIZnet device so we know how much data it
					has to send.  Later we will read the ack pointer and compare
					this to the Tx pointer to ascertain whether the transmission
					has completed. */

					/* First read the shadow register. */
					prvReadRegister( &ucShadow, tcpTX_WRITE_SHADOW_REG, tcpSHADOW_READ_LEN );

					/* Now a short delay is required. */
					vTaskDelay( tcpSHORT_DELAY );

					/* Then we can read the real register. */
					prvReadRegister( ( unsigned char * ) &ulWritePointer, tcpTX_WRITE_POINTER_REG, sizeof( ulWritePointer ) );

					/* We cannot do anything more here but need to remember that
					this interrupt has occurred. */
					lDataSent = pdTRUE;
				}

				if( ucISR & tcpISR_CLOSED )
				{
					/* The socket has been closed so we can leave this function. */
					lTransactionCompleted = pdFALSE;
				}
			}
			else
			{
				/* We have not received an interrupt from the WIZnet device for a
				while.  Read the socket status and check that everything is as
				expected. */
				prvReadRegister( &ucState, tcpSOCKET_STATE_REG, tcpSTATUS_READ_LEN );

				if( ( ucState == tcpSTATUS_ESTABLISHED ) && ( lDataSent > 0 ) )
				{
					/* The socket is established and we have already received a Tx
					end interrupt.  We must therefore be waiting for the Tx buffer
					inside the WIZnet device to be empty before we can close the
					socket.

					Read the Ack pointer register to see if it has caught up with
					the Tx pointer register.  First we have to read the shadow
					register. */
					prvReadRegister( &ucShadow, tcpTX_ACK_SHADOW_REG, tcpSHADOW_READ_LEN );
					vTaskDelay( tcpSHORT_DELAY );
					prvReadRegister( ( unsigned char * ) &ulAckPointer, tcpTX_ACK_POINTER_REG, sizeof( ulWritePointer ) );

					if( ulAckPointer == ulWritePointer )
					{
						/* The Ack and write pointer are now equal and we can
						safely close the socket. */
						i2cMessage( ucDataDisconnect, sizeof( ucDataDisconnect ), tcpDEVICE_ADDRESS, tcpCOMMAND_REG, i2cWRITE, NULL, portMAX_DELAY );
					}
					else
					{
						/* Keep a count of how many times we encounter the Tx
						buffer still containing data. */
						lDataSent++;
						if( lDataSent > tcpMAX_ATTEMPTS_TO_CHECK_BUFFER )
						{
							/* Assume we cannot complete sending the data and
							therefore cannot safely close the socket.  Start over. */
							vTCPHardReset();
							lTransactionCompleted = pdFALSE;
						}
					}
				}
				else if( ucState != tcpSTATUS_LISTEN )
				{
					/* If we have not yet received a Tx end interrupt we would only
					ever expect to find the socket still listening for any
					sustained period. */
					if( ucState == ucLastState )
					{
						lSameStateCount++;
						if( lSameStateCount > tcpMAX_NON_LISTEN_STAUS_READS )
						{
							/* We are persistently in an unexpected state.  Assume
							we cannot safely close the socket and start over. */
							vTCPHardReset();
							lTransactionCompleted = pdFALSE;
						}
					}
				}
				else
				{
					/* We are in the listen state so are happy that everything
					is as expected. */
					lSameStateCount = 0;
				}

				/* Remember what state we are in this time around so we can check
				for a persistence on an unexpected state. */
				ucLastState = ucState;
			}
		}

		/* We are going to reinitialise the WIZnet device so do not want our
		interrupts from the WIZnet to be processed. */
		VICIntEnClear |= tcpEINT0_VIC_CHANNEL_BIT;
		return lTransactionCompleted;
}

void vTCPListen( void )
{
		/* Then start listening for incoming connections. */
	vTWIMessage( ucDataListen, sizeof( ucDataListen ), tcpDEVICE_ADDRESS, tcpCOMMAND_REG, TWIWrite);
}
