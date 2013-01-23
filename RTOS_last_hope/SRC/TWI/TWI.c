#include <TWI.h>
#include <lib_AT91SAM7S256.h>

void AT91F_SetTwiClock(void)
{
	int iSclock;
	/* Here, CKDIV = 1 and CHDIV=CLDIV  ==> CLDIV = CHDIV = 1/4*((Fmclk/FTWI) -6)*/
	iSclock = (10 * MCK / AT91C_TWI_CLOCK);
	if (iSclock % 10 >= 5)
	{
		iSclock = (iSclock /10) - 5;
	}
	else
		{
			iSclock = (iSclock /10)- 6;
		}

	iSclock = (iSclock + (4 - iSclock % 4)) >> 2;	// div 4
    AT91C_BASE_TWI->TWI_CWGR = (1 << 16) | (iSclock << 8) | iSclock  ;
}

void vTWIMessage(const AT91PS_TWI pTwi, const unsigned char * const pucData, long lDataLength, unsigned char ucSlaveAddr, unsigned long ulDirection, int iBuffAddr)
{
	unsigned int uiCounter;

	// Set the TWI Master Mode Register
	pTwi->TWI_MMR = ucSlaveAddr | (ulDirection << MREAD_BIT) | AT91C_TWI_IADRSZ_1_BYTE;
	// Set TWI Internal Address Register
	pTwi->TWI_IADR = iBuffAddr;

	if (ulDirection == TWIRead)
	{
	// Start transfer
	if (lDataLength == 1)
	{
		pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_STOP;

		while (!(pTwi->TWI_SR & AT91C_TWI_TXCOMP));
		//*(pucData) = pTwi->TWI_RHR;
	}
 	else
 		{
 			pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN;

 			uiCounter = 0;
 			// Wait transfer is finished
 			while (!((pTwi->TWI_SR) & AT91C_TWI_TXCOMP))
 			{
 				if((pTwi->TWI_SR) & AT91C_TWI_RXRDY)
 				{
 				//	*(pucData+uiCounter++) = pTwi->TWI_RHR;
 					if (uiCounter == (lDataLength - 1))
 					{
 						pTwi->TWI_CR = AT91C_TWI_STOP;
 					}
 				}
 			}
 		}
	}
	else
		{
		if(lDataLength == 1)
			{
				pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN | AT91C_TWI_STOP;
				pTwi->TWI_THR = *pucData;

			}
			else
				{
					// Set the TWI Master Mode Register
					for(uiCounter = 0; uiCounter < lDataLength; uiCounter++)
					{
						pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN;
						if (uiCounter == (lDataLength - 1))
						{
							pTwi->TWI_CR = AT91C_TWI_STOP;
						}
						while (!((pTwi->TWI_SR) & AT91C_TWI_TXRDY));
						pTwi->TWI_THR = *(pucData+uiCounter);
					}
				}
				while (!(( pTwi->TWI_SR) & AT91C_TWI_TXCOMP));
		}
}

void AT91F_TWI_Open(void)
{
	// Configure TWI PIOs
	AT91F_TWI_CfgPIO ();
	// Configure PMC by enabling TWI clock
	AT91F_TWI_CfgPMC ();
	// Configure TWI in master mode
	AT91F_TWI_Configure (AT91C_BASE_TWI);
	// Set TWI Clock Waveform Generator Register
	AT91F_SetTwiClock();
}
