/*-----------------------------------------------------------------------------
*      ATMEL Microcontroller Software Support  -  ROUSSET  -
*------------------------------------------------------------------------------
* The software is delivered "AS IS" without warranty or condition of any
* kind, either express, implied or statutory. This includes without
* limitation any warranty or condition with respect to merchantability or
* fitness for any particular purpose, or against the infringements of
* intellectual property rights of others.
*------------------------------------------------------------------------------
* File Name         : lib_twi.c
* Object            : Basic TWI function driver
* Translator        :
* 1.0 25/11/02 NL   : Creation
* 1.1 31/Jan/05 JPP : Clean For basic
* 1.1 20/Oct/06 PFi : Twi bus clock parameter added to the AT91F_TWI_Open
*                   : function and in AT91F_SetTwiClock
* 1.1 23/Nov/06 PFi : Twi reset added in TWI Open function
* 1.2 19/Dec/06 PFi : Twi Bus recovery fnuction added.
*                   : configuration of TWD/TWCK in open drain mode
*                   : added in TWI_Open fct.
* 1.3 16/Mar/07 PFi : AT91F_SetTwiClock rewrite to set any clock with automatic
*                   : CKDIV computing.
*                   : All TWI read/write functions rewrite to match the new
*                   : flowcharts
*
* TO DO             : Add checking of (cldiv x 2pow*ckdiv) < 8192 in
*                     AT91F_SetTwiClock function according to errata the errata.
-----------------------------------------------------------------------------*/
#include "include\sam7p.h"
#include "include\twi.h"



#define AT91C_TWI_ACK 0
#define AT91C_TWI_TXCOMP ((unsigned int) 0x1 <<  0) // TWI_TXCOMP Transmission Complete
#define AT91C_TWI_RXRDY ((unsigned int) 0x1 <<  1) // TWI_RXRDY One byte has been received
#define AT91C_TWI_TXRDY ((unsigned int) 0x1 <<  2) // TWI_TXRDY One byte has been shifted out
//#define AT91C_TWI_ONE_BYTE_INTERNAL_ADDRESS ((unsigned int) 0x1 <<  8) // TWI_RXRDY A byte has been received

/* value of the PIT value to have 333ns @ 48 MHz
3 -1 since real time out = PIV + 1 */
#define PIT_PIV_MICRO_SECOND_VALUE  0x2

int TWI_TX_Comp = 0;
int TWI_RX_Ready = 0;
int TWI_TX_Ready = 0;
int TWI_Ack = AT91C_TWI_NACK;

void AT91F_TWI_Open(int TwiClock);


int TWI_WriteSingleIadr(int ModuleAddress,int Reg,int Value){

	volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;
	int TWI_Return, iCnt = 0;

	pTWI->TWI_MMR = (ModuleAddress << 15) | AT91C_TWI_IADRSZ_1_BYTE ;
	pTWI->TWI_CR = AT91C_TWI_START | AT91C_TWI_STOP;//AT91C_TWI_MSEN |
	pTWI->TWI_IADR = Reg;
	TWI_TX_Comp = 0;
	pTWI->TWI_THR = Value;				// send
	pTWI->TWI_IER = AT91C_TWI_TXCOMP; // start interrupt
	while(!TWI_TX_Comp && iCnt++ < 10000);	// wait until interrupt executed to check ack
	if(iCnt>10000){
		TWI_Return = ERROR_TWI;
	}
	else{
		TWI_Return = TWI_Ack;
	}
	return TWI_Return;
}

int TWI_ReadSingleIadr(int ModuleAddress,int Reg,int *Value){

	volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;
	int TWI_Return, iCnt = 0;

	pTWI->TWI_MMR = (ModuleAddress << 15) | (1 << 8) | (1 << 12);
	pTWI->TWI_IADR = Reg;
	TWI_RX_Ready = 0;

	pTWI->TWI_CR = AT91C_TWI_START | AT91C_TWI_STOP;
	pTWI->TWI_IER = AT91C_TWI_RXRDY;
	while(!TWI_RX_Ready && iCnt++ < 1000000);
	if(iCnt>1000000){
		TWI_Return = ERROR_TWI;
		*Value = 0;
	}
	else{
		TWI_Return = TWI_Ack;
		*Value = pTWI->TWI_RHR & 0xff;
	}
	return TWI_Return;
}


void TWI_IrqHandler (void) {

	unsigned int Status_Reg, Mask_Reg;
	volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;		// create a pointer to TWI structure

	Mask_Reg = pTWI->TWI_IMR;	//Interrupt Mask Register

	if(Mask_Reg & AT91C_TWI_TXRDY){
		pTWI->TWI_IDR = AT91C_TWI_TXRDY; // disable interrupt
		TWI_TX_Ready = 1;
		return;
	}
	else if(Mask_Reg & AT91C_TWI_TXCOMP){ // Transmission complete
		pTWI->TWI_IDR = AT91C_TWI_TXCOMP; // disable interrupt
		TWI_TX_Comp = 1;
	}
	else if(Mask_Reg & AT91C_TWI_RXRDY){ // Received data
		pTWI->TWI_IDR = AT91C_TWI_RXRDY; // disable interrupt
		TWI_RX_Ready = 1;
	}

	Status_Reg = pTWI->TWI_SR;	// Status Register
	if(Status_Reg & AT91C_TWI_NACK){
		TWI_Ack = AT91C_TWI_NACK;
	}
	else{
		TWI_Ack = AT91C_TWI_ACK;
	}
}

int TWI_ReadSingleIadrMultData(unsigned char Device, unsigned char Reg, unsigned char *Vec, unsigned char Size){

	volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;
	int i, TWI_Return=0, iCnt = 0;

	pTWI->TWI_MMR = (Device << 15) | AT91C_TWI_IADRSZ_1_BYTE | AT91C_TWI_MREAD;
	pTWI->TWI_IADR = Reg;

	pTWI->TWI_CR = AT91C_TWI_START;			// start
	for(i = 0;i < Size;i++){
		TWI_RX_Ready = 0;
		pTWI->TWI_IER = AT91C_TWI_RXRDY;; // start interrupt
		if(i+1==Size)
			pTWI->TWI_CR = AT91C_TWI_STOP;
		while(!TWI_RX_Ready && iCnt++ < 10000);	// wait until interrupt executed
		if(iCnt>10000){
			TWI_Return |= ERROR_TWI_OVERRUN;
			Vec[i] = 0;
		}
		else{
			TWI_Return |= TWI_Ack;
			Vec[i] = pTWI->TWI_RHR & 0xff;
		}
	}
	iCnt = 0;
	TWI_TX_Comp = 0;
	pTWI->TWI_IER = AT91C_TWI_TXCOMP; // start interrupt
	while(!TWI_TX_Comp && iCnt++ < 10000);	// wait until interrupt executed to check transmission done
	if(iCnt>10000){
		TWI_Return |= ERROR_TWI;
	}
	return TWI_Return;
}


int TWI_WriteSingleIadrMultData(unsigned char Module, unsigned char Reg, unsigned char *Vec, unsigned char Size){

	volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;
	int i, TWI_Return=0, iCnt = 0;

	pTWI->TWI_MMR = (Module << 15) | AT91C_TWI_IADRSZ_1_BYTE;
	pTWI->TWI_CR = AT91C_TWI_START;
	pTWI->TWI_IADR = Reg;

	for(i = 0;i < Size;i++)
	{
		TWI_TX_Ready = 0;
		iCnt = 0;
		pTWI->TWI_THR = Vec[i];				// send
		pTWI->TWI_IER = AT91C_TWI_TXRDY; // start interrupt
		while(!TWI_TX_Ready && iCnt++ < 1000);	// wait until interrupt executed
		if(iCnt>=1000)
		{
			pTWI->TWI_IDR = AT91C_TWI_TXRDY; // disable interrupt
			TWI_Return |= ERROR_TWI_OVERRUN;
			TWI_Init();
			break;
		}
	}
	if(iCnt < 1000)
	{
		iCnt = 0;
		TWI_TX_Comp = 0;
		pTWI->TWI_IER = AT91C_TWI_TXCOMP; // start interrupt
		while(!TWI_TX_Comp && iCnt++ < 1000);	// wait until interrupt executed to check ack
		if(iCnt>=1000){
			pTWI->TWI_IDR = AT91C_TWI_TXRDY; // disable interrupt
			TWI_Return |= ERROR_TWI;
		}
		else{
			TWI_Return |= TWI_Ack;
		}
	}
	return TWI_Return;
}


void Reset_TWI(){

	volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;
	// pointer to PIO data structure
	volatile AT91PS_PIO	pPIO = AT91C_BASE_PIOA;
	int i,j,Tmp;

	// PIO Enable Register - allow PIO to control pins P0 - P3 and pin 19
	pPIO->PIO_PER = TWI_MASK;
	// PIO Output Enable Register - sets pins P0 - P3 to outputs
	pPIO->PIO_OER = TWI_MASK;

	for(i=0;i<16;i++){
		pPIO->PIO_SODR = TWI_CLK;
		for(j=0;j<1000;j++);
		pPIO->PIO_CODR = TWI_CLK;
		for(j=0;j<1000;j++);
	}

	pTWI->TWI_CR = AT91C_TWI_SWRST;
//	pTWI->TWI_CR = AT91C_TWI_MSEN;
	TWI_Init();
}


void AT91F_PIO_CfgPeriph(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int periphAEnable,  // \arg PERIPH A to enable
	unsigned int periphBEnable)  // \arg PERIPH B to enable

{
	pPio->PIO_ASR = periphAEnable;
	pPio->PIO_BSR = periphBEnable;
	pPio->PIO_PDR = (periphAEnable | periphBEnable); // Set in Periph mode
}


void AT91F_TWI_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA4_TWCK    ) |
		((unsigned int) AT91C_PA3_TWD     ), // Peripheral A
		0); // Peripheral B
}

void AT91F_PMC_EnablePeriphClock (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int periphIds)  // \arg IDs of peripherals to enable
{
	pPMC->PMC_PCER = periphIds;
}


void AT91F_TWI_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_TWI));
}

void AT91F_TWI_Configure ( AT91PS_TWI pTWI )          // \arg pointer to a TWI controller
{
	int Status;
    //* Disable interrupts
	pTWI->TWI_IDR = (unsigned int) -1;
    //* Reset peripheral
	pTWI->TWI_CR = AT91C_TWI_SWRST;
	//* Set Master mode
	pTWI->TWI_CR = AT91C_TWI_MSEN;
	Status = pTWI->TWI_SR;
}

/*----------------------------------------------------------------------------
Function: AT91F_SetTwiClock (int TwiClock)
Arguments: - <TwiClock> TWI bus clock in Hertz
Comments : TO DO:

Return Value: none
-----------------------------------------------------------------------------*/
void AT91F_SetTwiClock(int TwiClock)
{
  unsigned int cldiv,ckdiv=1 ;

  /* CLDIV = ((Tlow x 2^CKDIV) -3) x Tmck */
  /* CHDIV = ((THigh x 2^CKDIV) -3) x Tmck */
  /* Only CLDIV is computed since CLDIV = CHDIV (50% duty cycle) */

  while ( ( cldiv = ( (MCK/(2*TwiClock))-3 ) / (1 << ckdiv)) > 255 )
   ckdiv++ ;

  AT91C_BASE_TWI->TWI_CWGR =(ckdiv<<16)|((unsigned int)cldiv << 8)|(unsigned int)cldiv  ;
}



/*-----------------------------------------------------------------------------
* \fn    AT91F_TWI_Open
* \brief Initializes TWI device
-----------------------------------------------------------------------------*/
void AT91F_TWI_Open(int TwiClock)
{
    /* Configure TWI PIOs */
    AT91F_TWI_CfgPIO ();

    /* Configure PMC by enabling TWI clock */
    AT91F_TWI_CfgPMC ();

    /* Configure TWI in master mode */
    AT91F_TWI_Configure (AT91C_BASE_TWI);

    /* Set TWI Clock Waveform Generator Register */
    AT91F_SetTwiClock(TwiClock);
}


void TWI_Init(void){

	volatile AT91PS_AIC	pAIC = AT91C_BASE_AIC;			// pointer to AIC data structure
	volatile AT91PS_TWI pTWI = AT91C_BASE_TWI;

	AT91F_TWI_Open(50000);
	// Set up the Advanced Interrupt Controller (AIC)  registers for TWI
	pAIC->AIC_IDCR = (1<<AT91C_ID_TWI);					// Disable TWI interrupt in AIC Interrupt Disable Command Register
	pAIC->AIC_SVR[AT91C_ID_TWI] =						// Set the TWI IRQ handler address in AIC Source
	   	(unsigned int)TWI_IrqHandler;        			// Vector Register[9]
	pAIC->AIC_SMR[AT91C_ID_TWI] =						// Set the interrupt source type(level-sensitive) and
		(AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | 0x4 ); 		// priority (4)in AIC Source Mode Register[6]
	pAIC->AIC_ICCR = (1<<AT91C_ID_TWI);					// Clear the interrupt on the interrupt controller
	pAIC->AIC_IECR = (1<<AT91C_ID_TWI); 				// Enable the TWI interrupt in AIC Interrupt Enable Command Register
}
