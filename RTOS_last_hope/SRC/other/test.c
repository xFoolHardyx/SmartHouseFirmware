#include <test.h>
#include <TWI.h>
#include <AT91SAM7X256.h>

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

#define byteslct AT91C_TWI_IADRSZ_2_BYTE


void test (void * pvParametrs)
{
//		TWIMessage( unsigned char * pucMessage,1
//		long lMessageLength,1
//		unsigned char ucSlaveAddress,1
//		unsigned char ucCountIntAddr,1
//		unsigned uBufferAddress,1
//		unsigned char ucDirection,
//		unsigned long ulTicksToWait);1

		TWIMessage( ucDataEnableISR, (long) sizeof( ucDataEnableISR ), tcpDEVICE_ADDRESS, byteslct, tcpISR_MASK_REG, (unsigned char)0, (unsigned long)0);
		TWIMessage( ucDataReset,(long)	sizeof( ucDataReset ),	tcpDEVICE_ADDRESS, byteslct, tcpCOMMAND_REG,(unsigned char)0, (unsigned long)0);
		TWIMessage( ucDataSHAR,	(long) sizeof( ucDataSHAR ),	tcpDEVICE_ADDRESS, byteslct, tcpSOURCE_HA_REG,(unsigned char)0, (unsigned long)0 );
		TWIMessage( ucDataGAR,	(long)	sizeof( ucDataGAR ),	tcpDEVICE_ADDRESS, byteslct, tcpGATEWAY_ADDR_REG,(unsigned char)0, (unsigned long)0);
		TWIMessage( ucDataMSR,	(long)	sizeof( ucDataMSR ),	tcpDEVICE_ADDRESS, byteslct, tcpSUBNET_MASK_REG,(unsigned char)0, (unsigned long)0);
		TWIMessage( ucDataSIPR,	(long) sizeof( ucDataSIPR ),	tcpDEVICE_ADDRESS, byteslct, tpcSOURCE_IP_REG,(unsigned char)0, (unsigned long)0);
		TWIMessage( ucDataSetTxBufSize, (long) sizeof( ucDataSetTxBufSize ), tcpDEVICE_ADDRESS, byteslct, tcpTX_MEM_SIZE_REG,(unsigned char)0, (unsigned long)0);
		TWIMessage( ucDataSetRxBufSize, (long) sizeof( ucDataSetRxBufSize ), tcpDEVICE_ADDRESS, byteslct, tcpRX_MEM_SIZE_REG,(unsigned char)0, (unsigned long)0);
		TWIMessage( ucDataInit,	(long) sizeof( ucDataInit ),	tcpDEVICE_ADDRESS, byteslct, tcpCOMMAND_REG,(unsigned char)0, (unsigned long)0);

		for (;;)
		{
			vTaskDelay(1000);
		}
}
