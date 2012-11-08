#include <AT91SAM7X256.h>
#include <settings.h>

void InitFrec (void)
{
	AT91PS_WDTC pWDTC = AT91C_BASE_WDTC; 	// WatchDog base addr
	AT91PS_PMC 	pPMC = AT91C_BASE_PMC;  	// Power Manager Controller base addr

	DisWD(pWDTC);
	SetMCK(pPMC,6);
	SetPLLnDrv(pPMC,14,72,28);
	SetPCLK(pPMC,2);
}

void InitPerepherial (void)
{
	AT91PS_PMC 	p_pPMC 	= AT91C_BASE_PMC;  // PMC for perepherial
	AT91PS_PIO	p_pPIO	= AT91C_BASE_PIOA; // Base PIOA

	/**** LED BUTTONS ****/
	PerCLKEn(p_pPMC,AT91C_ID_PIOA);

	  //LED 1
	  //configure the PIO Lines corresponding to LED1
	  p_pPIO->PIO_PER |= BIT18;    //Enable PA17
	  p_pPIO->PIO_OER |= BIT18;    //Configure in Output
	  p_pPIO->PIO_SODR |= BIT18;   //set reg to 0

	  //LED 2
	  //configure the PIO Lines corresponding to LED2
	  p_pPIO->PIO_PER |= BIT17;    //Enable PA18
	  p_pPIO->PIO_OER |= BIT17;    //Configure in Output
	  p_pPIO->PIO_SODR |= BIT17;   //set reg to 0
}
