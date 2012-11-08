#ifndef SETTINGS_H_
#define SETTINGS_H_

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

//////////////////////////////////////////////////////////////////////////
//	Disable WatchDog
//////////////////////////////////////////////////////////////////////////
#define DisWD(pWDTC) (pWDTC)->WDTC_WDMR = AT91C_WDTC_WDDIS

//////////////////////////////////////////////////////////////////////////
//	SetMCK
//	Set MCK at 48 054 850 Hz
//	1 Enabling the Main Oscillator:
//	LowCLK = 32768
//	SCK = 1/LowSCK = 0,000030517578125 Second
//	Start up time = 8 * 6 * SCK = 48 * 0,000030517578125 = 1,46484375 ms
//////////////////////////////////////////////////////////////////////////
#define SetMCK(pPMC,MUL) { 												    \
	(pPMC)->PMC_MOR =((AT91C_CKGR_OSCOUNT & (MUL<<8)) | AT91C_CKGR_MOSCEN); \
	while (!((pPMC)->PMC_SR & AT91C_PMC_MOSCS));}

//////////////////////////////////////////////////////////////////////////
//	SetPLLAndDriverCLK
//	2 Checking the Main Oscillator Frequency (Optional)
//	3 Setting PLL and divider:
//	DIV			: (18,432 / 14) = 1.3165
// 	MUL mul+1	: (1.3165 * 73) = 96.1097
// 	for 96 MHz the erroe is 0.11%
// 	Field out NOT USED = 0
// 	PLLCOUNT pll startup time estimate at : 0.844 ms
// 	PLLCOUNT 0.000844 /(1/32768) = 28
//////////////////////////////////////////////////////////////////////////
#define SetPLLnDrv(pPMC,div,mul,cnt) {						\
	(pPMC)->PMC_PLLR = ((AT91C_CKGR_DIV & (div<<1))| 		\
						(AT91C_CKGR_PLLCOUNT & (cnt<<8))| 	\
						(AT91C_CKGR_MUL & (mul<<16)));		\
	while(!((pPMC)->PMC_SR & AT91C_PMC_LOCK)); 				\
	while(!((pPMC)->PMC_SR & AT91C_PMC_MCKRDY));}

//////////////////////////////////////////////////////////////////////////
//	SetPrescalerCLK
//	4. select the master clock source and the prescalar
// 	source = the PLL clock
// 	prescalar = 2
//////////////////////////////////////////////////////////////////////////
#define SetPCLK(pPMC,PresCLK) {									\
	switch (PresCLK) {											\
	case 2	: (pPMC)->PMC_MCKR = AT91C_PMC_PRES_CLK_2; 	break;	\
	case 4	: (pPMC)->PMC_MCKR = AT91C_PMC_PRES_CLK_4; 	break;	\
	case 8	: (pPMC)->PMC_MCKR = AT91C_PMC_PRES_CLK_8; 	break;	\
	case 16	: (pPMC)->PMC_MCKR = AT91C_PMC_PRES_CLK_16; break;	\
	case 32	: (pPMC)->PMC_MCKR = AT91C_PMC_PRES_CLK_32; break;	\
	case 64	: (pPMC)->PMC_MCKR = AT91C_PMC_PRES_CLK_64; break;	\
	default : (pPMC)->PMC_MCKR = AT91C_PMC_PRES_CLK; 	break;}	\
	while(!((pPMC)->PMC_SR & AT91C_PMC_MCKRDY));				\
	(pPMC)->PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK;					\
	while(!((pPMC)->PMC_SR & AT91C_PMC_MCKRDY));}

/////////////////////////////////////////////////////////////////////////
//	PerepherialCLKEnable
/////////////////////////////////////////////////////////////////////////
#define PerCLKEn(pPMC,PerefID) (pPMC)->PMC_PCER = 1 << PerefID

/////////////////////////////////////////////////////////////////////////
// configure the PIO Lines corresponding to LED (in or out)
/////////////////////////////////////////////////////////////////////////
#define ConfPIO(pPIO,PIN,inp_out) {		\
	if (inp_out==0) { 					\
	(pPIO)->PIO_PER |= PIN;				\
	(pPIO)->PIO_OER |= PIN;				\
	(pPIO)->PIO_CODR |= PIN;} 			\
	else { 								\
		(pPIO)->PIO_PER |= PIN;			\
		(pPIO)->PIO_OER |= PIN;			\
		(pPIO)->PIO_SODR |= PIN;}}		\

void InitFrec(void);


#endif /* SETTINGS_H_ */
