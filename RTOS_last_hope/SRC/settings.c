#include <AT91SAM7X256.h>
#include <settings.h>
//#include <TWI.h>

//void InitFrec (void)
//{
//	AT91PS_WDTC pWDTC = AT91C_BASE_WDTC; 	// WatchDog base addr
//	AT91PS_PMC 	pPMC = AT91C_BASE_PMC;  	// Power Manager Controller base addr
//
//	DisWD(pWDTC);
//	SetMCK(pPMC,6);
//	SetPLLnDrv(pPMC,14,72,28);
//	SetPCLK(pPMC,2);
//}

void vInitPerepherial (void)
{
	AT91PS_PMC 	p_pPMC 	= AT91C_BASE_PMC;  // PMC for perepherial
	AT91PS_PIO	p_pPIO	= AT91C_BASE_PIOA; // Base PIOA

	/**** LED BUTTONS ****/
	PerCLKEn(p_pPMC,AT91C_ID_PIOA);

	//LED 1
	ConfPIO(p_pPIO,BIT18,0);
	//LED 2
	ConfPIO(p_pPIO,BIT17,0);
	//LED 2
	ConfPIO(p_pPIO,BIT16,0);

	//TWI init
	//vTWIInit();
}
