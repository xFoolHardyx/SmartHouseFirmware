#include "twi.h"

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Configures a TWI peripheral to operate in master mode, at the given
/// frequency (in Hz). The duty cycle of the TWI clock is set to 50%.
/// \param pTwi  Pointer to an AT91S_TWI instance.
/// \param twck  Desired TWI clock frequency.
/// \param mck  Master clock frequency.
//------------------------------------------------------------------------------
void TWI_Configure(AT91S_TWI *pTwi, unsigned int twck, unsigned int mck)
{
    unsigned int ckdiv = 0;
    unsigned int cldiv;
    unsigned char ok = 0;

    // Reset the TWI
    pTwi->TWI_CR = AT91C_TWI_SWRST;

    // Set master mode
    pTwi->TWI_CR = AT91C_TWI_MSEN;

    // Configure clock
    while (!ok) {
        
        cldiv = ((mck / (2 * twck)) - 3) / power(2, ckdiv);
        if (cldiv <= 255) {

            ok = 1;
        }
        else {

            ckdiv++;
        }
    }

    pTwi->TWI_CWGR = (ckdiv << 16) | (cldiv << 8) | cldiv;
}

void TWI_Stop(AT91S_TWI *pTwi)
{
    pTwi->TWI_CR = AT91C_TWI_STOP;
}

void TWI_StartRead(AT91S_TWI *pTwi, unsigned char address, unsigned int iaddress, unsigned char isize)
{
    // Set slave address and number of internal address bytes
    pTwi->TWI_MMR = (isize << 8) | AT91C_TWI_MREAD | (address << 16);

    // Set internal address bytes
    pTwi->TWI_IADR = iaddress;

    // Send START condition
    pTwi->TWI_CR = AT91C_TWI_START;
}

unsigned char TWI_ReadByte(AT91S_TWI *pTwi)
{
      return pTwi->TWI_RHR;
}

void TWI_WriteByte(AT91S_TWI *pTwi, unsigned char byte)
{
    pTwi->TWI_THR = byte;
}

void TWI_StartWrite(AT91S_TWI *pTwi, unsigned char address, unsigned int iaddress, unsigned char isize, unsigned char byte)
{
    // Set slave address and number of internal address bytes
    pTwi->TWI_MMR = (isize << 8) | (address << 16);

    // Set internal address bytes
    pTwi->TWI_IADR = iaddress;

    // Write first byte to send
    TWI_WriteByte(pTwi, byte);
}

//-----------------------------------------------------------------------------
/// Returns 1 if a byte has been received and can be read on the given TWI
/// peripheral; otherwise, returns 0. This function resets the status register
/// of the TWI.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//-----------------------------------------------------------------------------
unsigned char TWI_ByteReceived(AT91S_TWI *pTwi)
{
    return ((pTwi->TWI_SR & AT91C_TWI_RXRDY) == AT91C_TWI_RXRDY);
}

//-----------------------------------------------------------------------------
/// Returns 1 if a byte has been sent, so another one can be stored for
/// transmission; otherwise returns 0. This function clears the status register
/// of the TWI.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//-----------------------------------------------------------------------------
unsigned char TWI_ByteSent(AT91S_TWI *pTwi)
{
    return ((pTwi->TWI_SR & AT91C_TWI_TXRDY) == AT91C_TWI_TXRDY);
}

//-----------------------------------------------------------------------------
/// Returns 1 if the current transmission is complete (the STOP has been sent);
/// otherwise returns 0.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//-----------------------------------------------------------------------------
unsigned char TWI_TransferComplete(AT91S_TWI *pTwi)
{
    return ((pTwi->TWI_SR & AT91C_TWI_TXCOMP) == AT91C_TWI_TXCOMP);
}

//-----------------------------------------------------------------------------
/// Enables the selected interrupts sources on a TWI peripheral.
/// \param pTwi  Pointer to an AT91S_TWI instance.
/// \param sources  Bitwise OR of selected interrupt sources.
//-----------------------------------------------------------------------------
void TWI_EnableIt(AT91S_TWI *pTwi, unsigned int sources)
{
     pTwi->TWI_IER = sources;
}

//-----------------------------------------------------------------------------
/// Disables the selected interrupts sources on a TWI peripheral.
/// \param pTwi  Pointer to an AT91S_TWI instance.
/// \param sources  Bitwise OR of selected interrupt sources.
//-----------------------------------------------------------------------------
void TWI_DisableIt(AT91S_TWI *pTwi, unsigned int sources)
{
    pTwi->TWI_IDR = sources;
}

unsigned int TWI_GetStatus(AT91S_TWI *pTwi)
{
    return pTwi->TWI_SR;
}

//-----------------------------------------------------------------------------
/// Returns the current status register of the given TWI peripheral, but
/// masking interrupt sources which are not currently enabled.
/// This resets the internal value of the status register, so further read may
/// yield different values.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//-----------------------------------------------------------------------------
unsigned int TWI_GetMaskedStatus(AT91S_TWI *pTwi)
{
    unsigned int status;

    status = pTwi->TWI_SR;
    status &= pTwi->TWI_IMR;

    return status;
}

