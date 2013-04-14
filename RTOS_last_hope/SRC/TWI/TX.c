#include <TX.h>
#include <FreeRTOS.h>
#include <TWI.h>
#include <AT91SAM7X256.h>
#include <queue.h>
#include <task.h>


extern xReadyMessage xFlags;
static xQueueHandle xMessagesForTx;

void vTransmitData (void)
{
	volatile xTWIMessage *pxCurrentMessage = NULL;
	volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;
	int i, TWI_Return=0;// iCnt = 0;

	for(;;)
	{
		if (uxQueueMessagesWaiting(xMessagesForTx) != 0)
		{
//=============================================================================================================================
			xQueueReceive(xMessagesForTx, &pxCurrentMessage, 0);


				pTWI->TWI_MMR = pxCurrentMessage->ucDevAddr | pxCurrentMessage->ucInternalSizeAddr | pxCurrentMessage->ucDirection;
				pTWI->TWI_CR = AT91C_TWI_START;
				pTWI->TWI_IADR = pxCurrentMessage->uInternalAddr;

				for(i = 0; i<pxCurrentMessage->lMessageLength; i++)
				{
					xFlags->TWI_RX_Ready = TWI_FALSE;

//					iCnt = 0;
					pTWI->TWI_THR = pxCurrentMessage->pucBuf[i];	// send
					pTWI->TWI_IER = AT91C_TWI_TXRDY; 				// start interrupt
					while(!xFlags->TWI_RX_Ready /*&& iCnt++ < 1000*/);			// wait until interrupt executed
//					if(iCnt>=1000)
//					{
//						pTWI->TWI_IDR = AT91C_TWI_TXRDY; 			// disable interrupt
//						TWI_Return |= ERROR_TWI_OVERRUN;
//						TWI_Init();
//						break;
//					}
				}
//				if(iCnt < 1000)
//				{
//					iCnt = 0;

					xFlags->TWI_TX_Comp = TWI_FALSE;

					pTWI->TWI_IER = AT91C_TWI_TXCOMP;		// start interrupt

					while(!xFlags->TWI_TX_Comp /*&& iCnt++ < 1000*/);	// wait until interrupt executed to check ack
//					if(iCnt>=1000){
//						pTWI->TWI_IDR = AT91C_TWI_TXRDY; 	// disable interrupt
//						TWI_Return |= ERROR_TWI;
//					}
//					else
//					{
//						TWI_Return |= TWI_Ack;
//					}
//				}
//				return TWI_Return;
//=============================================================================================================================

		}
		else vTaskDelay(100);
	}
}
