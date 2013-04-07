#include <TX.h>
#include <TWI_ISR_MY.h>
#include <AT91SAM7X256.h>


extern xReadyMessage xFlags;

//unsigned int uiMessageTWI(unsigned int ucSlct, unsigned int uiRegAddr, unsigned char *SendBuf, unsigned int uiLength)

//bus variable - true or false

void vTransmitData (void)
{
	for(;;)
	{
		if ((xFlags->TWI_TX_Ready) && (pulBusFree == pTrue)
		{
			unsigned int uiStatus;
			unsigned int i;
			unsigned int error = 0;

			AT91PS_TWI pTwi = AT91C_BASE_TWI;

			// Set TWI Internal Address Register
			if ((ucSlct & AT91C_TWI_IADRSZ) != 0)
				{
					pTwi->TWI_IADR = uiRegAddr; //reg addr
				}

			// Set the TWI Master Mode Register
			pTwi->TWI_MMR = ucSlct & ~AT91C_TWI_MREAD;

			if(uiLength < 2)
			{
				pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN | AT91C_TWI_STOP;
				pTwi->TWI_THR = *SendBuf;
			}
			else
			{
			// Set the TWI Master Mode Register
			  for(i=0; i<uiLength; i++)
			  {
				  pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN;
				  if (i == (uiLength - 1))
					{
						pTwi->TWI_CR = AT91C_TWI_STOP;
					}

				  uiStatus = pTwi->TWI_SR;

				  if ((uiStatus & ERROR) == ERROR)
				  {
					error++;
				  }

				  while (!(uiStatus & AT91C_TWI_TXRDY))
				  {
					   uiStatus = pTwi->TWI_SR;
					   if ((uiStatus & ERROR) == ERROR) error++;
				  }
				  pTwi->TWI_THR = *(SendBuf+i);
			   }
			}
			uiStatus = pTwi->TWI_SR;
			if ((uiStatus & ERROR) == ERROR) error++;
			while (!(uiStatus & AT91C_TWI_TXCOMP)){
					uiStatus = pTwi->TWI_SR;
					if ((uiStatus & ERROR) == ERROR) error++;
			}
			return error;
		}
	}
}
