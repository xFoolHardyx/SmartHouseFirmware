//  ***********************************************************************************************************
//    lowlevelinit.c
//
//     Basic hardware initialization
//
// SLCK = 42000 hz (worst case)  32768 hz is the nominal slow clock frequency
// SLCK_PERIOD = 1 / 42000 = 23.8 usec
//
// MAINCK  = 18432000 hz crystal on Olimex SAM7-EX256 board)
// PLLCK = (MAINCK / DIV) * (MUL + 1) = 18432000/14 * (72 + 1)
// PLLCLK = 1316571 * 73 = 96109683 hz
// MCK = PLLCLK / 2 = 96109683 / 2 = 48054841 hz
//
// Note: see page 5 - 6 of Atmel's "Getting Started with AT91SAM7X Microcontrollers" for details.
//
//  Author:  James P Lynch  June 22, 2008
//  ***********************************************************************************************************
// include files
#include "include/AT91SAM7S128.h"
#include "include/Board.h"
// external references
//extern void AT91F_Spurious_handler(void);
extern void AT91F_Default_IRQ_handler(void);
extern void AT91F_Default_FIQ_handler(void);

void LowLevelInit(void)
{
	int i;
	AT91PS_PMC pPMC = AT91C_BASE_PMC;

//  Set Flash Wait state
//
// Note: MCK period = 1 / 48054841 hz = 20.0809 nsec
// FMCN = number of Master clock cycles in 1 microsecond = 1.0 usec/ 20.08095 nsec = 50 (rounded up)
//
// FWS = flash wait states = 1 for 48 Mhz operation (FWS = 1)
// note: see page 656 of AT91SAM7XC512/256/128 Preliminary User Guide
//
//  result: 0xFFFFFF60 = 0x00300100  (AT91C_BASE_MC->MC_FMR = MC Flash Mode Register)
	AT91C_BASE_MC->MC_FMR = ((AT91C_MC_FMCN) & (50 << 16)) | AT91C_MC_FWS_1FWS;
	//  Watchdog Disable
	//
	// result: 0xFFFFFD44 = 0x00008000  (AT91C_BASE_WDTC->WDTC_WDMR = Watchdog Mode Register)
	AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;
	// Enable the Main Oscillator
	//
	// Give the Main Oscillator 1.5 msec to start up
	// Main oscillator startup time = SlowCockPeriod * 8 * OSCOUNT
	// SlowClockPeriod = 1 / 42000 = .0000238 sec  (worst case RC clock)
// OSCOUNT =  8
// MOS startup time = 23.8 usec * 8 * OSCOUNT = .0000238 * 8 * 8 = 1.5 msec
//
// MOSCEN = 1 (enables main oscillator)
//
// result: 0xFFFFFC20 = 0x00000801  (pPMC->PMC_MOR = Main Oscillator Register)
	pPMC->PMC_MOR = ((AT91C_CKGR_OSCOUNT & (0x08 << 8)) | AT91C_CKGR_MOSCEN);
// Wait the startup time (until PMC Status register MOSCEN bit is set)
// result: 0xFFFFFC68 bit 0 will set when main oscillator has stabilized
	while (!(pPMC->PMC_SR & AT91C_PMC_MOSCS))
		;
// PMC Clock Generator PLL Register setup
//
// The following settings are used:  DIV = 14
//                                   MUL = 72
//                                   PLLCOUNT = 10
//
// Main Clock (MAINCK from crystal oscillator) = 18432000 hz (see AT91SAM7-EK schematic)
// Note: input freq to PLL must be 1 Mhz to 32 Mhz so 18.432 Mhz is OK
//
// MAINCK / DIV = 18432000/14 = 1316571 hz
// PLLCK = 1316571 * (MUL + 1) = 1316571 * (72 + 1) = 1316571 * 73 = 96109683 hz
//
// PLLCOUNT = number of slow clock cycles before the LOCK bit is set in PMC_SR after CKGR_PLLR is written.
//
// PLLCOUNT = 10
//
// OUT = 0 (sets allowable range of PLL output freq from 80 Mhz to 160 Mhz ---> 96.109683 Mhz is OK)
//
// result: 0xFFFFFC2C = 0x0000000048200E   (pPMC->PMC_PLLR = PLL Register)
	pPMC->PMC_PLLR =
			((AT91C_CKGR_OUT_0) | (AT91C_CKGR_DIV & 14)
					| (AT91C_CKGR_PLLCOUNT & (40 << 10))
					| (AT91C_CKGR_MUL & (72 << 16)));
// Wait the startup time (until PMC Status register LOCK bit is set)
// result: 0xFFFFFC68 bit 2 will set when PLL has locked
	while (!(pPMC->PMC_SR & AT91C_PMC_LOCK))
		;

// PMC Master Clock (MCK) Register setup
//
// CSS  = 3  (PLLCK clock selected)
//
// PRES = 1  (MCK = PLLCK / 2) = 96109683/2 = 48054841 hz
//
// Note: Master Clock  MCK = 48054841 hz  (this is the CPU clock speed)
// result:  0xFFFFFC30 = 0x00000004  (pPMC->PMC_MCKR = Master Clock Register)
	pPMC->PMC_MCKR = AT91C_PMC_PRES_CLK_2;
// Wait the startup time (until PMC Status register MCKRDY bit is set)
// result: 0xFFFFFC68 bit 3 will set when Master Clock has stabilized
	while (!(pPMC->PMC_SR & AT91C_PMC_MCKRDY))
		;
// result:  0xFFFFFC30 = 0x00000007  (pPMC->PMC_MCKR = Master Clock Register)
	pPMC->PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK;
// Wait the startup time (until PMC Status register MCKRDY bit is set)
// result: 0xFFFFFC68 bit 3 will set when Master Clock has stabilized
	while (!(pPMC->PMC_SR & AT91C_PMC_MCKRDY))
		;
// Set up the default interrupts handler vectors
	AT91C_BASE_AIC->AIC_SVR[0] = (int) AT91F_Default_FIQ_handler;
	for (i = 1; i < 31; i++) {
		AT91C_BASE_AIC->AIC_SVR[i] = (int) AT91F_Default_IRQ_handler;
	}
}
