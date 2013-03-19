#include <lib.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 															TWI
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void TWI_Configure(unsigned int uiTwck, unsigned int uiMck)
//{
//	AT91PS_TWI pTwi = AT91C_BASE_TWI;
//
//	unsigned int uiCkdiv = 0;
//    unsigned int uiCldiv;
//    unsigned char ucOk = 0;
//
//    // Reset the TWI
//    pTwi->TWI_CR = AT91C_TWI_SWRST;
//
//    // Set master mode
//    pTwi->TWI_CR = AT91C_TWI_MSEN;
//
//    // Configure clock
//    while (!ucOk) {
//
//        uiCldiv = ((uiMck / (2 * uiTwck)) - 3) / pow(2, uiCkdiv);
//        if (uiCldiv <= 255) {
//
//            ucOk = 1;
//        }
//        else {
//
//            uiCkdiv++;
//        }
//    }
//    pTwi->TWI_CWGR = (uiCkdiv << 16) | (uiCldiv << 8) | uiCldiv;
//}

//------------------------------------------------------------------------------
/// Sends a STOP condition on the TWI.
//------------------------------------------------------------------------------
void TWI_Stop()
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;
    pTwi->TWI_CR = AT91C_TWI_STOP;
}

//------------------------------------------------------------------------------
/// Starts a read operation on the TWI bus with the specified slave, and returns
/// immediately. Data must then be read using TWI_ReadByte() whenever a byte is
/// available (poll using TWI_ByteReceived()).
/// address  	Slave address on the bus.
/// iaddress 	Optional internal address bytes.
/// isize  		Number of internal address bytes.
//-----------------------------------------------------------------------------
void TWI_StartRead(unsigned char ucAddress, unsigned int uiIaddress, unsigned char ucIsize)
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;

    // Set slave address and number of internal address bytes
    pTwi->TWI_MMR = (ucIsize << 8) | AT91C_TWI_MREAD | (ucAddress << 16);

    // Set internal address bytes
    pTwi->TWI_IADR = uiIaddress;

    // Send START condition
    pTwi->TWI_CR = AT91C_TWI_START;
}

//-----------------------------------------------------------------------------
/// Reads a byte from the TWI bus. The read operation must have been started
/// using TWI_StartRead() and a byte must be available (check with
/// TWI_ByteReceived()).
/// Returns the byte read.
//-----------------------------------------------------------------------------
unsigned char TWI_ReadByte()
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;
	return pTwi->TWI_RHR;
}

//-----------------------------------------------------------------------------
/// Sends a byte of data to one of the TWI slaves on the bus. This function
/// must be called once before TWI_StartWrite() with the first byte of data
/// to send, then it shall be called repeatedly after that to send the
/// remaining bytes.
/// byte  Byte to send.
//-----------------------------------------------------------------------------
void TWI_WriteByte(unsigned char ucByte)
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;
	pTwi->TWI_THR = ucByte;
}

//-----------------------------------------------------------------------------
/// Starts a write operation on the TWI to access the selected slave, then
/// returns immediately. A byte of data must be provided to start the write;
/// other bytes are written next.
/// address  Address of slave to acccess on the bus.
/// iaddress  Optional slave internal address.
/// isize  Number of internal address bytes.
/// byte  First byte to send.
//-----------------------------------------------------------------------------
void TWI_StartWrite( unsigned char ucAddress, unsigned int uiIaddress, unsigned char ucIsize, unsigned int uiLength, unsigned char * ucByte)
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;

	unsigned int i;


    // Set slave address and number of internal address bytes
    pTwi->TWI_MMR = (ucIsize << 8) | (ucAddress << 16);

    // Set internal address bytes
    pTwi->TWI_IADR = uiIaddress;

    pTwi->TWI_CR = AT91C_TWI_START;
    if (uiLength < 2)
    {
    // Write first byte to send
    TWI_WriteByte(ucByte[0]);
    }else
    {
    	for (i = 0; i < uiLength; i++)
    	{
    		TWI_WriteByte(ucByte[i]);
    	}
    }
}

unsigned int uiMessageTWI(unsigned int ucSlct, unsigned int uiRegAddr, unsigned char *SendBuf, unsigned int uiLength)
{
	unsigned int uiStatus;
	unsigned int i;
	unsigned int error = 0;

	AT91PS_TWI pTwi = AT91C_BASE_TWI;

	// Set TWI Internal Address Register
	if ((ucSlct & AT91C_TWI_IADRSZ) != 0)
		{
			pTwi->TWI_IADR = uiRegAddr; //reg addr
		}

	// Set the TWI Master Mode Register
	pTwi->TWI_MMR = ucSlct & ~AT91C_TWI_MREAD;

	if(uiLength < 2)
	{
		pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN | AT91C_TWI_STOP;
		pTwi->TWI_THR = *SendBuf;
	}
	else
	{
	// Set the TWI Master Mode Register
	  for(i=0; i<uiLength; i++)
	  {
          pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN;
          if (i == (uiLength - 1))
          	{
          		pTwi->TWI_CR = AT91C_TWI_STOP;
          	}

          uiStatus = pTwi->TWI_SR;

//          if ((uiStatus & ERROR) == ERROR)
//          {
//          	error++;
//          }

          while (!(uiStatus & AT91C_TWI_TXRDY))
          {
               uiStatus = pTwi->TWI_SR;
//               if ((uiStatus & ERROR) == ERROR) error++;
          }
          pTwi->TWI_THR = *(SendBuf+i);
	   }
	}
	uiStatus = pTwi->TWI_SR;
//	if ((uiStatus & ERROR) == ERROR) error++;
	while (!(uiStatus & AT91C_TWI_TXCOMP)){
    		uiStatus = pTwi->TWI_SR;
//    		if ((uiStatus & ERROR) == ERROR) error++;
    }
	return error;
}

//-----------------------------------------------------------------------------
/// Returns 1 if a byte has been received and can be read on the given TWI
/// peripheral; otherwise, returns 0. This function resets the status register
/// of the TWI.
//-----------------------------------------------------------------------------
unsigned char TWI_ByteReceived()
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;
    return ((pTwi->TWI_SR & AT91C_TWI_RXRDY) == AT91C_TWI_RXRDY);
}

//-----------------------------------------------------------------------------
/// Returns 1 if a byte has been sent, so another one can be stored for
/// transmission; otherwise returns 0. This function clears the status register
/// of the TWI.

//-----------------------------------------------------------------------------
unsigned char TWI_ByteSent()
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;
    return ((pTwi->TWI_SR & AT91C_TWI_TXRDY) == AT91C_TWI_TXRDY);
}

//-----------------------------------------------------------------------------
/// Returns 1 if the current transmission is complete (the STOP has been sent);
/// otherwise returns 0.
//-----------------------------------------------------------------------------
unsigned char TWI_TransferComplete()
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;
    return ((pTwi->TWI_SR & AT91C_TWI_TXCOMP) == AT91C_TWI_TXCOMP);
}

//-----------------------------------------------------------------------------
/// Enables the selected interrupts sources on a TWI peripheral.
/// sources  Bitwise OR of selected interrupt sources.
//-----------------------------------------------------------------------------
void TWI_EnableIt(unsigned int uiSources)
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;
    pTwi->TWI_IER = uiSources;
}

//-----------------------------------------------------------------------------
/// Disables the selected interrupts sources on a TWI peripheral.
/// sources  Bitwise OR of selected interrupt sources.
//-----------------------------------------------------------------------------
void TWI_DisableIt(unsigned int uiSources)
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;
    pTwi->TWI_IDR = uiSources;
}

//-----------------------------------------------------------------------------
/// Returns the current status register of the given TWI peripheral. This
/// resets the internal value of the status register, so further read may yield
/// different values.
//-----------------------------------------------------------------------------
unsigned int TWI_GetStatus()
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;
	return pTwi->TWI_SR;
}

//-----------------------------------------------------------------------------
/// Returns the current status register of the given TWI peripheral, but
/// masking interrupt sources which are not currently enabled.
/// This resets the internal value of the status register, so further read may
/// yield different values.
//-----------------------------------------------------------------------------
unsigned int TWI_GetMaskedStatus()
{
	AT91PS_TWI pTwi = AT91C_BASE_TWI;
    unsigned int uiStatus;

    uiStatus = pTwi->TWI_SR;
    uiStatus &= pTwi->TWI_IMR;

    return uiStatus;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 															AIC
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//*----------------------------------------------------------------------------
//* Interrupt Handler Initialization
// \arg interrupt number to initialize
// \arg priority to give to the interrupt
// \arg activation and sense of activation
// \arg address of the interrupt handler
//*----------------------------------------------------------------------------
unsigned int AIC_ConfigureIt (unsigned int uiIrq_id, unsigned int uiPriority, unsigned int uiSrc_type, void (*newHandler) () )
{
	AT91PS_AIC pAic = AT91C_BASE_AIC;
	unsigned int uiOldHandler;
    unsigned int uiMask ;

    uiOldHandler = pAic->AIC_SVR[uiIrq_id];

    uiMask = 0x1 << uiIrq_id ;
    //* Disable the interrupt on the interrupt controller
    pAic->AIC_IDCR = uiMask ;
    //* Save the interrupt handler routine pointer and the interrupt priority
    pAic->AIC_SVR[uiIrq_id] = (unsigned int) newHandler ;
    //* Store the Source Mode Register
    pAic->AIC_SMR[uiIrq_id] = uiSrc_type | uiPriority  ;
    //* Clear the interrupt on the interrupt controller
    pAic->AIC_ICCR = uiMask ;

	return uiOldHandler;
}

//*----------------------------------------------------------------------------
//* Enable corresponding IT number
// \arg interrupt number to initialize
//*----------------------------------------------------------------------------
void AIC_EnableIt (unsigned int uiIrq_id)
{
	AT91PS_AIC pAic = AT91C_BASE_AIC;
    //* Enable the interrupt on the interrupt controller
    pAic->AIC_IECR = 0x1 << uiIrq_id ;
}

//*----------------------------------------------------------------------------
//* Disable corresponding IT number
// \arg interrupt number to initialize
//*----------------------------------------------------------------------------
void AIC_DisableIt (unsigned int uiIrq_id )
{
	AT91PS_AIC pAic = AT91C_BASE_AIC;

    unsigned int uiMask = 0x1 << uiIrq_id;
    //* Disable the interrupt on the interrupt controller
    pAic->AIC_IDCR = uiMask ;
    //* Clear the interrupt on the Interrupt Controller ( if one is pending )
    pAic->AIC_ICCR = uiMask ;
}

//*----------------------------------------------------------------------------
//* Clear corresponding IT number
// \arg interrupt number to initialize
//*----------------------------------------------------------------------------
void AIC_ClearIt ( unsigned int uiIrq_id)
{
	AT91PS_AIC pAic = AT91C_BASE_AIC;
    //* Clear the interrupt on the Interrupt Controller ( if one is pending )
    pAic->AIC_ICCR = (0x1 << uiIrq_id);
}

//*----------------------------------------------------------------------------
//* Configure vector handler
// \arg pointer to the AIC registers
// \arg Interrupt Handler
//*----------------------------------------------------------------------------
unsigned int AIC_SetExceptionVector (unsigned int *pVector, void (*Handler) () )
{
	unsigned int uiOldVector = *pVector;

	if ((unsigned int) Handler == (unsigned int) AT91C_AIC_BRANCH_OPCODE)
		*pVector = (unsigned int) AT91C_AIC_BRANCH_OPCODE;
	else
		*pVector = (((((unsigned int) Handler) - ((unsigned int) pVector) - 0x8) >> 2) & 0x00FFFFFF) | 0xEA000000;

	return uiOldVector;
}

//*----------------------------------------------------------------------------
//* Trig an IT
// \arg interrupt number
//*----------------------------------------------------------------------------
void AIC_Trig (unsigned int uiIrq_id)
{
	AT91PS_AIC pAic = AT91C_BASE_AIC;
	pAic->AIC_ISCR = (0x1 << uiIrq_id);
}

//*----------------------------------------------------------------------------
//* Test if an IT is active
// \arg Interrupt Number
//*----------------------------------------------------------------------------
unsigned int AIC_IsActive (unsigned int uiIrq_id)
{
	AT91PS_AIC pAic = AT91C_BASE_AIC;
	return (pAic->AIC_ISR & (0x1 << uiIrq_id));
}

//*----------------------------------------------------------------------------
//* Test if an IT is pending
// \arg Interrupt Number
//*----------------------------------------------------------------------------
unsigned int AIC_IsPending (unsigned int uiIrq_id)
{
	AT91PS_AIC pAic = AT91C_BASE_AIC;
	return (pAic->AIC_IPR & (0x1 << uiIrq_id));
}

//*----------------------------------------------------------------------------
//* Set exception vectors and AIC registers to default values
// \arg Default IRQ vector exception
// \arg Default FIQ vector exception
// \arg Default Handler set in ISR
// \arg Default Spurious Handler
// \arg Debug Control Register
//*----------------------------------------------------------------------------
void AIC_Open(void (*IrqHandler) (), void (*FiqHandler) (), void (*DefaultHandler)  (), void (*SpuriousHandler) (), unsigned int protectMode)
{
	AT91PS_AIC pAic = AT91C_BASE_AIC;
	int i;

	// Disable all interrupts and set IVR to the default handler
	for (i = 0; i < 32; ++i) {
		AT91F_AIC_DisableIt(pAic, i);
		AIC_ConfigureIt(i, AT91C_AIC_PRIOR_LOWEST, AT91C_AIC_SRCTYPE, DefaultHandler);
	}

	// Set the IRQ exception vector
	AIC_SetExceptionVector((unsigned int *) 0x18, IrqHandler);
	// Set the Fast Interrupt exception vector
	AIC_SetExceptionVector((unsigned int *) 0x1C, FiqHandler);

	pAic->AIC_SPU = (unsigned int) SpuriousHandler;
	pAic->AIC_DCR = protectMode;
}

//*----------------------------------------------------------------------------
//* Enable Peripheral clock in PMC for  AIC
//*----------------------------------------------------------------------------
void AIC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock( ((unsigned int) 1 << AT91C_ID_IRQ0) | ((unsigned int) 1 << AT91C_ID_FIQ) | ((unsigned int) 1 << AT91C_ID_IRQ1));
}

//*----------------------------------------------------------------------------
//* Configure PIO controllers to drive AIC signals
//*----------------------------------------------------------------------------
void AIC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(((unsigned int) AT91C_PA30_IRQ1), ((unsigned int) AT91C_PA20_IRQ0) | ((unsigned int) AT91C_PA19_FIQ));
}

