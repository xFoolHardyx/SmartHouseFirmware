//#include "include/FreeRTOS.h"
//#include "include/task.h"

//#include "include/flash.h"
//#include "include/queue.h"
#include  "hardware/include/Board.h"


#define   BIT0        0x00000001
#define   BIT1        0x00000002
#define   BIT2        0x00000004
#define   BIT3        0x00000008
#define   BIT4        0x00000010
#define   BIT5        0x00000020
#define   BIT6        0x00000040
#define   BIT7        0x00000080
#define   BIT8        0x00000100
#define   BIT9        0x00000200
#define   BIT10       0x00000400
#define   BIT11       0x00000800
#define   BIT12       0x00001000
#define   BIT13       0x00002000
#define   BIT14       0x00004000
#define   BIT15       0x00008000
#define   BIT16       0x00010000
#define   BIT17       0x00020000
#define   BIT18       0x00040000
#define   BIT19       0x00080000
#define   BIT20       0x00100000
#define   BIT21       0x00200000
#define   BIT22       0x00400000
#define   BIT23       0x00800000
#define   BIT24       0x01000000
#define   BIT25       0x02000000
#define   BIT26       0x04000000
#define   BIT27       0x08000000
#define   BIT28       0x10000000
#define   BIT29       0x20000000
#define   BIT30       0x40000000
#define   BIT31       0x80000000

AT91PS_PMC 	pPMC   	= AT91C_BASE_PMC;  // Power Manager Controller base addr
AT91PS_PMC 	p_pPMC 	= AT91C_BASE_PMC;  // PMC for perepherial
AT91PS_PIO	p_pPIO	= AT91C_BASE_PIOA; // Base PIOA
AT91PS_WDTC pWDTC 	= AT91C_BASE_WDTC; // WatchDog base addr


void InitFrec (void);
void InitPerepherial (void);
void delay (unsigned long a);

void main (void)
{
	InitFrec();
	InitPerepherial();
	const AT91PS_PIO m_pPIOA = AT91C_BASE_PIOA;
	while (1)
		{
			 m_pPIOA->PIO_CODR = BIT17;  //set reg to 0 (led2 on)
			 m_pPIOA->PIO_SODR = BIT18;  //set reg to 1 (led1 off)
			 delay(800000);             //simple delay
			 m_pPIOA->PIO_CODR = BIT18;  //set reg to 0 (led1 on)
			 m_pPIOA->PIO_SODR = BIT17;  //set reg to 1 (led2 off)
			 delay(800000);             //simple delay
		}
}



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

void delay (unsigned long a) {while (--a!=0);}
