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

int AT91F_TWI_ReadByte(const AT91PS_TWI pTwi, int mode, int int_address, char *data, int nb)
{
	unsigned int uiStatus;
	unsigned int uiCounter = 0;
	unsigned int uiError   = 0;

	// Set TWI Internal Address Register
	if ((mode & AT91C_TWI_IADRSZ) != 0)
	{
		pTwi->TWI_IADR = int_address;
	}

	// Set the TWI Master Mode Register
	pTwi->TWI_MMR = mode | AT91C_TWI_MREAD;
	// Start transfer
	if (nb == 1)
	{
		pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_STOP;
		uiStatus = pTwi->TWI_SR;
		if ((uiStatus & ERROR) == ERROR)
		{
			uiError++;
		}

		while (!(uiStatus & AT91C_TWI_TXCOMP))
		{
			uiStatus = pTwi->TWI_SR;
            if ((uiStatus & ERROR) == ERROR)
            {
            	uiError++;
            }
    	}

		*(data) = pTwi->TWI_RHR;
	}
 	else
 		{
 			pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN;
 			uiStatus = pTwi->TWI_SR;
 			if ((uiStatus & ERROR) == ERROR)
 			{
 				uiError++;
 			}
 			// Wait transfer is finished
 			while (!(uiStatus & AT91C_TWI_TXCOMP))
 			{
 				uiStatus = pTwi->TWI_SR;
 				if ((uiStatus & ERROR )== ERROR)
 				{
 					uiError++;
 				}
 				if(uiStatus & AT91C_TWI_RXRDY)
 				{
 					*(data+uiCounter++) = pTwi->TWI_RHR;
 					if (uiCounter == (nb - 1))
 					{
 						pTwi->TWI_CR = AT91C_TWI_STOP;
 					}
 				}
 			}
 		}
return 0;
}

int iTWIMessageW(const AT91PS_TWI pTwi, unsigned char *pucData, long lDataLegth, unsigned char ucSlaveAddr, unsigned long ulDirection, int iBuffAddr)
{
	unsigned int uiStatus;
	unsigned int uiCounter;
	unsigned int uiError	= 0;

	// Set the TWI Master Mode Register
	pTwi->TWI_MMR	= ucSlaveAddr | (ulDirection << MREAD_BIT) | AT91C_TWI_IADRSZ_1_BYTE;
	// Set TWI Internal Address Register
	pTwi->TWI_IADR 	= iBuffAddr;

	if(lDataLegth == 1)
	{
		pTwi->TWI_THR = *pucData;
		pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN | AT91C_TWI_STOP;
	}
	else
		{
			// Set the TWI Master Mode Register
			for(uiCounter = 0; uiCounter < lDataLegth; uiCounter++)
			{
				pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN;
				if (uiCounter == (lDataLegth - 1))
				{
					pTwi->TWI_CR = AT91C_TWI_STOP;
				}
				uiStatus = pTwi->TWI_SR;

				if ((uiStatus & NACK) == NACK)
				{
					uiError++;
				}

				while (!(uiStatus & AT91C_TWI_TXRDY))
				{
					uiStatus = pTwi->TWI_SR;
					if ((uiStatus & NACK) == NACK)
					{
						uiError++;
					}
				}
				pTwi->TWI_THR = *(pucData+uiCounter);
			}
		}
		uiStatus = pTwi->TWI_SR;
		if ((uiStatus & NACK) == NACK)
		{
			uiError++;
		}

		while (!(uiStatus & AT91C_TWI_TXCOMP))
		{
			uiStatus = pTwi->TWI_SR;
    		if ((uiStatus & NACK) == NACK)
    		{
    			uiError++;
    		}
		}
return uiError;
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
