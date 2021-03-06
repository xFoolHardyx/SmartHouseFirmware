#include <TWI.h>
#include <AT91SAM7X256.h>
#include <lib_AT91SAM7S256.h>
#include <FreeRTOS.h>

void vTWIClock(void)
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

void vTWIMessage(unsigned char * const pucData, long lDataLength, unsigned char ucSlaveAddr, int iBuffAddr, unsigned long ulDirection)
{
	unsigned int unCounter;
	unsigned int unStatus=0;
	const AT91PS_TWI pTwi;

	portENTER_CRITICAL();

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
		*(pucData) = pTwi->TWI_RHR;
	}
 	else
 		{
 			pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN;

 			unCounter = 0;
 			// Wait transfer is finished
 			while (!((pTwi->TWI_SR) & AT91C_TWI_TXCOMP))
 			{
 				if((pTwi->TWI_SR) & AT91C_TWI_RXRDY)
 				{
 					*(pucData+unCounter++) = pTwi->TWI_RHR;
 					if (unCounter == (lDataLength - 1))
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
					for(unCounter = 0; unCounter < lDataLength; unCounter++)
					{
						pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN;
						if (unCounter == (lDataLength - 1))
						{
							pTwi->TWI_CR = AT91C_TWI_STOP;
						}
						unStatus=pTwi->TWI_SR;
						while (!((pTwi->TWI_SR) & AT91C_TWI_TXRDY));
						pTwi->TWI_THR = *(pucData+unCounter);
					}
				}
				unStatus=pTwi->TWI_SR;
				while (!(( pTwi->TWI_SR) & AT91C_TWI_TXCOMP));
		}
	portEXIT_CRITICAL();
}

void vTWIInit(void)
{
	// Configure TWI PIOs
	AT91F_TWI_CfgPIO ();
	// Configure PMC by enabling TWI clock
	AT91F_TWI_CfgPMC ();
	// Configure TWI in master mode
	AT91F_TWI_Configure (AT91C_BASE_TWI);
	// Set TWI Clock Waveform Generator Register
	vTWIClock();
}
