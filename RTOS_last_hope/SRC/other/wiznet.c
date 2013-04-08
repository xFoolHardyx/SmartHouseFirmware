#include <FreeRTOS.h>
#include <task.h>
#include <lib.h>
#include <wiznet.h>
#include <settings.h>



#define tcpRESET_CMD					( ( unsigned char ) 0x80 )
#define tcpSYS_INIT_CMD					( ( unsigned char ) 0x01 )
#define tcpSOCK_STREAM					( ( unsigned char ) 0x01 )
#define tcpSOCK_INIT					( ( unsigned char ) 0x02 )
#define tcpLISTEN_CMD					( ( unsigned char ) 0x08 )
#define tcpRECEIVE_CMD					( ( unsigned char ) 0x40 )
#define tcpDISCONNECT_CMD				( ( unsigned char ) 0x10 )
#define tcpSEND_CMD						( ( unsigned char ) 0x20 )

#define tcpCLEAR_EINT0					( 1 )
#define i2cCLEAR_ALL_INTERRUPTS			( ( unsigned char ) 0xff )
#define i2cCHANNEL_0_ISR_ENABLE			( ( unsigned char ) 0x01 )
#define i2cCHANNEL_0_ISR_DISABLE		( ( unsigned char ) 0x00 )
#define tcpWAKE_ON_EINT0				( 1 )
#define tcpENABLE_EINT0_FUNCTION		( ( unsigned long ) 0x01 )
#define tcpEINT0_VIC_CHANNEL_BIT		( ( unsigned long ) 0x4000 )
#define tcpEINT0_VIC_CHANNEL			( ( unsigned long ) 14 )
#define tcpEINT0_VIC_ENABLE				( ( unsigned long ) 0x0020 )

#define tcp8K_RX						( ( unsigned char ) 0x03 )
#define tcp8K_TX						( ( unsigned char ) 0x03 )

#define tcpDEVICE_ADDRESS 				( ( unsigned char ) 0x00 )

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


#define tcpRESET_DELAY					( ( portTickType ) 16 / portTICK_RATE_MS )
#define tcpINIT_DELAY					( ( portTickType ) 500 / portTICK_RATE_MS  )

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

unsigned char ucDataGAR[]				= { 192, 168, 1, 1 };	/* Gateway address. */
unsigned char ucDataMSR[]				= { 255, 255, 255, 0 };	/* Subnet mask.		*/
unsigned char ucDataSIPR[]				= { 192, 168, 1, 131 };/* IP address.		*/
unsigned char ucDataSHAR[]				= { 00, 23, 30, 41, 15, 26 }; /* MAC address - DO NOT USE THIS ON A PUBLIC NETWORK! */



void sendset(void)
{
	AT91PS_PIO	p_pPIO	= AT91C_BASE_PIOA; // Base PIOA
	AT91PS_TWI pTWI = AT91C_BASE_TWI;
		SetBIT(p_pPIO,BIT25); // reset
		ClrBIT(p_pPIO,BIT26); // ~reset

		/* Delay with the network hardware in reset for a short while. */
		vTaskDelay( tcpRESET_DELAY );

	//	GPIO_IOCLR = tcpRESET_ACTIVE_HIGH; // set 4 bit in 0
	//	GPIO_IOSET = tcpRESET_ACTIVE_LOW;  // set 5 bit on 1
		ClrBIT(p_pPIO,BIT25); // reset
		SetBIT(p_pPIO,BIT26); // ~reset

		vTaskDelay( tcpINIT_DELAY );

		/* Setup the EINT0 to interrupt on required events from the WIZnet device.
		First enable the EINT0 function of the pin. */

	//	PCB_PINSEL1 |= tcpENABLE_EINT0_FUNCTION;
//		p_pPIO->PIO_PER = (1<<20);
//		p_pPIO->PIO_ASR = (1<<20);
//		pTWI->TWI_IER = AT91C_TWI_TXRDY; // start interrupt

	message( ucDataEnableISR, sizeof( ucDataEnableISR ), tcpDEVICE_ADDRESS, tcpISR_MASK_REG, 2);
	pTWI->TWI_IER = AT91C_TWI_TXRDY;

	message( ucDataReset,	sizeof( ucDataReset ),	tcpDEVICE_ADDRESS, tcpCOMMAND_REG, 2);

	message( ucDataSHAR,	sizeof( ucDataSHAR ),	tcpDEVICE_ADDRESS, tcpSOURCE_HA_REG, 2 );
	message( ucDataGAR,		sizeof( ucDataGAR ),	tcpDEVICE_ADDRESS, tcpGATEWAY_ADDR_REG, 2);
	message( ucDataMSR,		sizeof( ucDataMSR ),	tcpDEVICE_ADDRESS, tcpSUBNET_MASK_REG, 2);
	message( ucDataSIPR,	sizeof( ucDataSIPR ),	tcpDEVICE_ADDRESS, tpcSOURCE_IP_REG, 2);

	message( ucDataSetTxBufSize, sizeof( ucDataSetTxBufSize ), tcpDEVICE_ADDRESS, tcpTX_MEM_SIZE_REG, 2);
	message( ucDataSetRxBufSize, sizeof( ucDataSetRxBufSize ), tcpDEVICE_ADDRESS, tcpRX_MEM_SIZE_REG, 2);

	message( ucDataInit,		sizeof( ucDataInit ),	tcpDEVICE_ADDRESS, tcpCOMMAND_REG, 2);
}

void message (unsigned char * pucSrc, unsigned int uiLength, unsigned char ucDevAddr, unsigned int uiInDevAddr, unsigned int ucSlct)
{
	ucSlct = ucSlct << 8;
	uiMessageTWI(ucSlct, uiInDevAddr, pucSrc, uiLength);
//	TWI_StartWrite(ucDevAddr, uiInDevAddr, ucSlct, uiLength, pucSrc);
//	TWI_Stop();
}



