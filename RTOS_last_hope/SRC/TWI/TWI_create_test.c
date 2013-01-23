#include <TWI_create_test.h>

#define i2cCHANNEL_0_ISR_ENABLE			( ( unsigned char ) 0x01 )
#define tcpDEVICE_ADDRESS 				( ( unsigned char ) 0x00 )
#define tcpISR_MASK_REG					( ( unsigned short ) 0x0009 )
#define tcpRESET_CMD					( ( unsigned char ) 0x80 )
#define tcpCOMMAND_REG					( ( unsigned short ) 0x0000 )

#define tcpGATEWAY_ADDR_REG				( ( unsigned short ) 0x0080 )
#define tcpSUBNET_MASK_REG				( ( unsigned short ) 0x0084 )
#define tcpSOURCE_HA_REG				( ( unsigned short ) 0x0088 )
#define tpcSOURCE_IP_REG				( ( unsigned short ) 0x008E )

#define tcp8K_RX						( ( unsigned char ) 0x03 )
#define tcp8K_TX						( ( unsigned char ) 0x03 )

#define tcpTX_MEM_SIZE_REG				( ( unsigned short ) 0x0096 )
#define tcpRX_MEM_SIZE_REG				( ( unsigned short ) 0x0095 )

#define tcpSYS_INIT_CMD					( ( unsigned char ) 0x01 )

const unsigned char const ucDataEnableISR[]			= { i2cCHANNEL_0_ISR_ENABLE };
const unsigned char const ucDataReset[]				= { tcpRESET_CMD };

const unsigned char const ucDataGAR[]				= { 172, 25, 218, 3 };	/* Gateway address. */
const unsigned char const ucDataMSR[]				= { 255, 255, 255, 0 };	/* Subnet mask.		*/
const unsigned char const ucDataSIPR[]				= { 172, 25, 218, 201 };/* IP address.		*/
const unsigned char const ucDataSHAR[]				= { 00, 23, 30, 41, 15, 26 }; /* MAC address - DO NOT USE THIS ON A PUBLIC NETWORK! */

const unsigned char const ucDataSetTxBufSize[]		= { tcp8K_TX };
const unsigned char const ucDataSetRxBufSize[] 		= { tcp8K_RX };
const unsigned char const ucDataInit[]				= { tcpSYS_INIT_CMD };

void vTaskTWIStart(void *pvParameters)
{
	int a;

	vTWIMessage(AT91C_BASE_TWI, ucDataEnableISR, sizeof(i2cCHANNEL_0_ISR_ENABLE), tcpDEVICE_ADDRESS, TWIWrite, tcpISR_MASK_REG);

	vTWIMessage(AT91C_BASE_TWI, ucDataReset, sizeof(ucDataReset), tcpDEVICE_ADDRESS, TWIWrite, tcpCOMMAND_REG);
	vTWIMessage(AT91C_BASE_TWI, ucDataSHAR, sizeof(ucDataSHAR), tcpDEVICE_ADDRESS, TWIWrite, tcpSOURCE_HA_REG);
	vTWIMessage(AT91C_BASE_TWI, ucDataGAR, sizeof(ucDataGAR), tcpDEVICE_ADDRESS, TWIWrite, tcpGATEWAY_ADDR_REG);
	vTWIMessage(AT91C_BASE_TWI, ucDataMSR, sizeof(ucDataMSR), tcpDEVICE_ADDRESS, TWIWrite, tcpSUBNET_MASK_REG);
	vTWIMessage(AT91C_BASE_TWI, ucDataSIPR, sizeof(ucDataSIPR), tcpDEVICE_ADDRESS, TWIWrite, tpcSOURCE_IP_REG);
	vTWIMessage(AT91C_BASE_TWI, ucDataSetTxBufSize, sizeof(ucDataSetTxBufSize), tcpDEVICE_ADDRESS, TWIWrite, tcpTX_MEM_SIZE_REG);
	vTWIMessage(AT91C_BASE_TWI, ucDataSetRxBufSize, sizeof(ucDataSetRxBufSize), tcpDEVICE_ADDRESS, TWIWrite, tcpRX_MEM_SIZE_REG);
	vTWIMessage(AT91C_BASE_TWI, ucDataInit, sizeof(ucDataInit), tcpDEVICE_ADDRESS, TWIWrite, tcpCOMMAND_REG);

	for(;;)
	{
		a=a+a;
	}
}

void vTaskTWICreate(void)
{
	vTWIInit();
}

void vTWIInit(void)
{
	AT91F_TWI_Open();
}


