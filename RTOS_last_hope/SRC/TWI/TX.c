#include <TX.h>
#include <TWI_ISR_MY.h>
#include <AT91SAM7X256.h>


extern xReadyMessage xFlags;
static xQueueHandle xMessagesForTx;

//unsigned int uiMessageTWI(unsigned int ucSlct, unsigned int uiRegAddr, unsigned char *SendBuf, unsigned int uiLength)

//bus variable - true or false

void vTransmitData (void)
{
	for(;;)
	{
		if (uxQueueMessagesWaiting(xMessagesForTx) != 0)
		{

		}
		else vTaskDelay(100);
	}
}
