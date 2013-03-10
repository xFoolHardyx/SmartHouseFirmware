#ifndef TWI_H
#define TWI_H


#include <AT91SAM7X256.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 															TWI
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Returns 1 if the TXRDY bit (ready to transmit data) is set in the given
/// status register value.
#define TWI_STATUS_TXRDY(status) ((status & AT91C_TWI_TXRDY) == AT91C_TWI_TXRDY)

/// Returns 1 if the RXRDY bit (ready to receive data) is set in the given
/// status register value.
#define TWI_STATUS_RXRDY(status) ((status & AT91C_TWI_RXRDY) == AT91C_TWI_RXRDY)

/// Returns 1 if the TXCOMP bit (transfer complete) is set in the given
/// status register value.
#define TWI_STATUS_TXCOMP(status) ((status & AT91C_TWI_TXCOMP) == AT91C_TWI_TXCOMP)

extern void TWI_Configure(unsigned int uiTwck, unsigned int uiMck);

extern void TWI_Stop();

extern void TWI_StartRead(unsigned char ucAddress, unsigned int uiIaddress, unsigned char ucIsize);

extern unsigned char TWI_ReadByte();

extern void TWI_WriteByte(unsigned char ucByte);

extern void TWI_StartWrite( unsigned char ucAddress, unsigned int uiIaddress, unsigned char ucIsize, unsigned char ucByte);

extern unsigned char TWI_ByteReceived();

extern unsigned char TWI_ByteSent();

extern unsigned char TWI_TransferComplete();

extern void TWI_EnableIt(unsigned int uiSources);

extern void TWI_DisableIt(unsigned int uiSources);

extern unsigned int TWI_GetStatus();

extern unsigned int TWI_GetMaskedStatus();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 															AIC
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define AT91C_AIC_BRANCH_OPCODE ((void (*) ()) 0xE51FFF20) // ldr, pc, [pc, #-&F20]

extern unsigned int AIC_ConfigureIt (unsigned int uiIrq_id, unsigned int uiPriority, unsigned int uiSrc_type, void (*newHandler) () );

extern void AIC_EnableIt (unsigned int uiIrq_id);

extern void AIC_DisableIt (unsigned int uiIrq_id );

extern void AIC_ClearIt ( unsigned int uiIrq_id);

extern unsigned int AIC_SetExceptionVector (unsigned int *pVector, void (*Handler) () );

extern void AIC_Trig (unsigned int uiIrq_id);

extern unsigned int AIC_IsActive (unsigned int uiIrq_id);

extern unsigned int AIC_IsPending (unsigned int uiIrq_id);

extern void AIC_Open(void (*IrqHandler) (), void (*FiqHandler) (), void (*DefaultHandler)  (), void (*SpuriousHandler) (), unsigned int protectMode);

extern void AIC_CfgPMC (void);

extern void AIC_CfgPIO (void);

#endif //#ifndef TWI_H
