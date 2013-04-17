#include <TX.h>
#include <AT91SAM7X256.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <TWI_ISR_MY.h>
#include <TWI.h>

//extern xReadyMessage xFlags;
extern xQueueHandle xMessagesForTx;

extern unsigned char TWI_TX_Ready;
extern unsigned char TWI_TX_Comp;
extern unsigned char TWI_RX_Ready;

volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;

void vTransmitData (void *pvParameters)
{
	xTWIMessage * pxCurrentMessage = NULL;

	int TWI_Ack = AT91C_TWI_NACK;

	int i, TWI_Return=0, iCnt = 0;
	unsigned int rrr;
	unsigned portBASE_TYPE size_queue;

	for(;;)
	{
		size_queue= 0 ;
		size_queue = uxQueueMessagesWaiting(xMessagesForTx);
		if (size_queue != 0)
		{
//=============================================================================================================================
			xQueueReceive(xMessagesForTx, &pxCurrentMessage, 0);


				pTWI->TWI_MMR = pxCurrentMessage->ucDevAddr | pxCurrentMessage->ucInternalSizeAddr | pxCurrentMessage->ucDirection;
				pTWI->TWI_CR = AT91C_TWI_START;
				rrr=pTWI->TWI_CR = AT91C_TWI_START;
				pTWI->TWI_IADR = pxCurrentMessage->uInternalAddr;

				for(i = 0; i<pxCurrentMessage->lMessageLength + 1; i++)
				{
					TWI_RX_Ready = TWI_FALSE;

					iCnt = 0;
					pTWI->TWI_THR = pxCurrentMessage->pucBuf[i];	// send
					rrr=pTWI->TWI_THR;
					pTWI->TWI_IER = AT91C_TWI_TXRDY; 				// start interrupt
					while(!TWI_RX_Ready && iCnt++ < 1000);			// wait until interrupt executed
//					if(iCnt>=1000)
//					{
//						pTWI->TWI_IDR = AT91C_TWI_TXRDY; 			// disable interrupt
//						TWI_Return |= ERROR_TWI_OVERRUN;
//						TWI_Init();
//						break;
//					}
				}
				if(iCnt < 1000)
				{
					iCnt = 0;

					TWI_TX_Comp = TWI_FALSE;

					pTWI->TWI_IER = AT91C_TWI_TXCOMP;		// start interrupt

					while(!TWI_TX_Comp && iCnt++ < 1000);	// wait until interrupt executed to check ack
					if(iCnt>=1000){
						pTWI->TWI_IDR = AT91C_TWI_TXRDY; 	// disable interrupt
						TWI_Return |= ERROR_TWI;
					}
					else
					{
						TWI_Return |= TWI_Ack;
					}
				}
//				return TWI_Return;
//=============================================================================================================================

		}
		else vTaskDelay(100);
	}
}
