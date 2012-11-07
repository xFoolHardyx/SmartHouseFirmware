#include <AT91SAM7X256.h>
#include <settings.h>

AT91PS_PMC 	pPMC   	= AT91C_BASE_PMC;  // Power Manager Controller base addr
AT91PS_PMC 	p_pPMC 	= AT91C_BASE_PMC;  // PMC for perepherial
AT91PS_PIO	p_pPIO	= AT91C_BASE_PIOA; // Base PIOA
AT91PS_WDTC pWDTC 	= AT91C_BASE_WDTC; // WatchDog base addr

void InitFrec (void)
{
	pWDTC -> WDTC_WDMR = AT91C_WDTC_WDDIS; //disable WatchDog

	//* Set MCK at 48 054 850 Hz
	// 1 Enabling the Main Oscillator:
  	//SCK = 1/32768 = 30.51 uSecond
 	//Start up time = 8 * 6 / SCK = 48 * 30.51 = 1,46484375 ms
	pPMC->PMC_MOR = (( AT91C_CKGR_OSCOUNT & (0x06 <<8)) | AT91C_CKGR_MOSCEN ); //Power Manager Control Main Osc Reg

	while(!(pPMC->PMC_SR & AT91C_PMC_MOSCS)); //wait startup Main Oscellator status

	// 2 Checking the Main Oscillator Frequency (Optional)
	// 3 Setting PLL and divider:
	// - div by 14 Fin = 1.3165 =(18,432 / 14)
	// - Mul 72+1: Fout =	96.1097 =(3,6864 *73)
	// for 96 MHz the erroe is 0.11%
	// Field out NOT USED = 0
	// PLLCOUNT pll startup time estimate at : 0.844 ms
	// PLLCOUNT 28 = 0.000844 /(1/32768)

	pPMC->PMC_PLLR = ((AT91C_CKGR_DIV & (7<<1))|(AT91C_CKGR_PLLCOUNT & (28<<8))|(AT91C_CKGR_MUL & (72<<16))); //Divider Selected,PLL Counter,PLL Multiplier

	while(!(pPMC->PMC_SR & AT91C_PMC_LOCK)); // wait for the PLL to stabalize
	while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY)); // wait for the "master clock ready" flag

	// 4. select the master clock source and the prescalar
	// source    = the PLL clock
	// prescalar = 2
	pPMC->PMC_MCKR = AT91C_PMC_PRES_CLK_2; //set prescelar 2
	while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));
	pPMC->PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK; // PLL clock is selected
	while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));
}

void InitPerepherial (void)
{
	/**** LED BUTTONS ****/
	//enable the clock of the PIO
	p_pPMC->PMC_PCER = 1 << AT91C_ID_PIOA; //PMC Peripheral Clock Enable Register

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
