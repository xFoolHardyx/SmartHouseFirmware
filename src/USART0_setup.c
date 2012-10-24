// *********************************************************************************************************
//    usart0_setup.c
//
// Purpose:  Set up USART0  (peripheral ID = 6)   9600 baud, 8 data bits, 1 stop bit, no parity
//
// We will use the onboard baud rate generator to specify 9600 baud
//
// The Olimex SAM7-EX256 board has a 18,432,000 hz crystal oscillator.
//
// MAINCK  = 18432000 hz   (from Olimex schematic)
// DIV = 14                (set up in lowlevelinit.c)
// MUL = 72                (set up in lowlevelinit.c)
//
// PLLCK = (MAINCK / DIV) * (MUL + 1) = 18432000/14 * (72 + 1)
// PLLCLK = 1316571 * 73 = 96109683 hz
// MCK = PLLCLK / 2 = 96109683 / 2 = 48054841 hz
//
// Baud Rate (asynchronous mode) = MCK / (8(2 - OVER)CD)
//
// MCK   = 48054841 hz    (set USCLKS = 00 in USART Mode Register US_MR - to select MCK only)
// VER  = 0              (bit 19 of the USART Mode Register US_MR)
// CD    = divisor        (USART Baud Rate Generator Register US_BRGR)
// baudrate = 9600        (desired)
//AT91SAM7 Serial Communications                                                                                                 44 Of 91
//                                             48054841            48054841
// a little algebra:  BaudRate = --------------   =  -----------
//                                             (8(2 - 0)CD)          16(CD)
//
//                                48054841       48054841
//                         CD  = ----------  =  ---------- = 312.857037
//                                9600(16)        153600
//
//                         CD = 313  (round up)
//
//                                                           48054841         48054841
// check the actual baud rate: BaudRate = ---------------  =  ------------ =  9595.6
//                                                       (8 (2 - 0)313          5008
//
//                                       desired baudrate            9600
//      what's the error:   Error = 1 - ------------------  =  1 - ---------  =  1 - 1.00045854 = -.0004585
//                                       actual baudrate             9595.6                       (not much)
//
// Author:  James P Lynch  June 22, 2008
// **********************************************************************************************************
//  *******************************************************
//                Header Files
//  *******************************************************
#include "include/AT91SAM7S128.h"
#include "include/board.h"
//  *******************************************************
//                External Globals
//  *******************************************************
extern char Buffer[]; // holds received characters
extern unsigned long nChars; // counts number of received chars
extern char *pBuffer; // pointer into Buffer
//  *******************************************************
//                Function Prototypes
//  *******************************************************
void Usart0IrqHandler(void);
void USART0Setup(void) {
// enable the usart0 peripheral clock
volatile AT91PS_PMC pPMC = AT91C_BASE_PMC; // pointer to PMC data structure
pPMC->PMC_PCER = (1<<AT91C_ID_US0); // enable usart0 peripheral clock
// set up PIO to enable USART0 peripheral control of pins
volatile AT91PS_PIO pPIO = AT91C_BASE_PIOA; // pointer to PIO data structure
pPIO->PIO_PDR = AT91C_PA5_RXD0 | AT91C_PA6_TXD0; // enable peripheral control of PA0,PA1 (RXD0 and TXD0)
pPIO->PIO_ASR = AT91C_PIO_PA0 | AT91C_PIO_PA1; // assigns the 2 I/O lines to peripheral A function
pPIO->PIO_BSR = 0;                              // peripheral B function set to "no effect"
// set up the USART0 registers
volatile AT91PS_USART pUSART0 = AT91C_BASE_US0; // create a pointer to USART0 structure
// USART0 Control Register US_CR    (read/write)
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//   31                                                                   24
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                     RTSDIS   RTSEN    DTRDIS   DTREN  |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      23       22       21       20       19       18       17       16
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// | RETTO   RSTNACK   RSTIT    SENDA    STTTO    STPBRK   STTBRK   RSTSTA |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      15       14       13       12       11       10        9        8
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// | TXDIS    TXEN     RXDIS    RXEN     RSTTX    RSTRX      -        -    |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//       7          6      5        4        3        2        1        0
//
// RSTRX = 1 (reset receiver)
// RSTTX = 1 (reset transmitter)
// RXEN = 0 (receiver enable - no effect)
// RXDIS = 1 (receiver disable - disabled)
// TXEN = 0 (transmitter enable - no effect)
// TXDIS = 1 (transmitter disable - disabled)
// RSTSTA = 0 (reset status bits - no effect)
// STTBRK = 0 (start break - no effect)
// STPBRK = 0 (stop break - no effect)
// STTTO = 0 (start time-out - no effect)
// SENDA = 0 (send address - no effect)
// RSTIT = 0 (reet iterations - no effect)
// RSTNACK = 0 (reset non acknowledge - no effect)
// RETTO = 0 (rearm time-out - no effect)
// DTREN = 0 (data terminal ready enable - no effect)
// DTRDIS = 0 (data terminal ready disable - no effect)
// RTSEN = 0 (request to send enable - no effect)
// RSTDIS = 0 (request to send disable - no effect)
pUSART0->US_CR = AT91C_US_RSTRX |  // reset receiver
AT91C_US_RSTTX | // reset transmitter
AT91C_US_RXDIS | // disable receiver
AT91C_US_TXDIS; // disable transmitter
// USART0 Mode Register US_MR    (read/write)
//
// |--------------------------|--------|--------|--------------------------|
// |                            FILTER               MAX_ITERATION         |
// |--------------------------|--------|--------|--------------------------|
//      31                       29    28       27    26                      24
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                   DSNACK    INACK    OVER     CLK0     MODE9    MSBF  |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//          23       22       21       20       19       18       17       16
//
// |-----------------|-----------------|--------------------------|--------|
// |     CHMODE           NBSTOP                 PAR                 SYNC  |
// |-----------------|-----------------|--------------------------|--------|
//       15             14 13             12 11                       9     8
//
// |-----------------|-----------------|-----------------------------------|
// |      CHRL               USCLKS              USART_MODE                |
// |-----------------|-----------------|-----------------------------------|
//       7               6  5              4  3                                0
//
// USART_MODE = 0000   normal
// USCLKS = 00         choose MCK for baud rate generator
// CHRL = 11           8-bit characters
// SYNC = 0            asynchronous mode
// PAR = 100           no parity
// NBSTOP = 00         1 stop bit
// CHMODE = 00         normal mode, no loop-back, etc.
// MSBF = 0 LSB sent/received first
// MODE9 = 0           CHRL defines character length
// CLK0 = 0 USART does not drive SCK pin
// OVER = 0 16 x oversampling (see baud rate equation)
// INACK = 0 NACK (not used)
// DSNACK = 0 not used since NACK is not generated
// MAX_ITERATION = 0   max iterations not used
// FILTER = 0 filter is off
pUSART0->US_MR = AT91C_US_PAR_NONE | // no parity
0x3 << 6;  // 8-bit characters
// USART0 Interrupt Enable Register US_IER    (write only)
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                                                       |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      31       30       29       28       27       26       25       24
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                     CTSIC     DCDIC   DSRIC    RIIC   |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      23       22       21       20       19       18       17       16
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                    NACK    RXBUFF   TXBUFE  ITERATIO TXEMPTY  TIMEOUT |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      15       14       13       12       11       10        9        8
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |  PARE     FRAME    OVRE     ENDTX   ENDRX    RXBRK    TXRDY    RXRDY  |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//       7          6      5        4        3        2        1        0
//
pUSART0->US_IER = 0x00; // no usart0 interrupts enabled  (no effect)
// USART0 Interrupt Disable Register US_IDR    (write only)
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                                                       |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      31       30       29       28       27       26       25       24
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                     CTSIC     DCDIC   DSRIC    RIIC   |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      23       22       21       20       19       18       17       16
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                    NACK    RXBUFF   TXBUFE  ITERATIO TXEMPTY  TIMEOUT |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      15       14       13       12       11       10        9        8
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |  PARE     FRAME    OVRE     ENDTX   ENDRX    RXBRK    TXRDY    RXRDY  |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//       7          6      5        4        3        2        1        0
//
pUSART0->US_IDR = 0xFFFF; // disable all USART0 interrupts
// USART0 Interrupt Mask Register US_IDR    (write only)
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                                                       |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      31       30       29       28       27       26       25       24
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                     CTSIC     DCDIC   DSRIC    RIIC   |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      23       22       21       20       19       18       17       16
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                    NACK    RXBUFF   TXBUFE  ITERATIO TXEMPTY  TIMEOUT |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      15       14       13       12       11       10        9        8
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |  PARE     FRAME    OVRE     ENDTX   ENDRX    RXBRK    TXRDY    RXRDY  |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//       7          6      5        4        3        2        1        0
//  read only, nothing to set up here
// USART0 Receive Holding Register  US_RHR    (read only)
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       31                                                                   24
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       23                                                                   16
//
// |--------|-----------------------------------------------------|--------|
// | RXSYNH                                                          RXCHR |
// |--------|-----------------------------------------------------|--------|
//       15    14                                                     9     8
//
// |-----------------------------------------------------------------------|
// |                             RXCHR                                  Y  |
// |-----------------------------------------------------------------------|
//        7                                                                     0
//  this is where any incoming character will be
// USART0 Transmit Holding Register  US_THR    (write only)
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       31                                                                   24
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       23                                                                   16
//
// |--------|-----------------------------------------------------|--------|
// | TXSYNH                                                          TXCHR |
// |--------|-----------------------------------------------------|--------|
//       15    14                                                  9     8
//
// |-----------------------------------------------------------------------|
// |                             TXCHR                                  Y  |
// |-----------------------------------------------------------------------|
//       7                                                                     0
//  this is where we place characters to be transmitted
// USART0 Baud Rate Generator Register US_BRGR    (read/write)
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       31                                                                   24
//
// |--------------------------------------------|--------------------------|
// |                                                     FP                |
// |--------------------------------------------|--------------------------|
//       23                                        19 18                      16
//
// |-----------------------------------------------------------------------|
// |                              CD                                       |
// |-----------------------------------------------------------------------|
//       15                                                                    8
//
// |-----------------------------------------------------------------------|
// |                              CD                                    Y  |
// |-----------------------------------------------------------------------|
//        7                                                                     0
pUSART0->US_BRGR = 0x139; // CD = 0x139  (313 from above calculation)  FP=0 (not used)
// USART0 Receiver Time-out Register US_RTOR    (read/write)
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                                                       |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      31       30       29       28       27       26       25       24
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                                                       |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//      23       22       21       20       19       18       17       16
//
// |-----------------------------------------------------------------------|
// |                               TO                                      |
// |-----------------------------------------------------------------------|
//       15                                                                    9
//
// |-----------------------------------------------------------------------|
// |                               TO                                      |
// |-----------------------------------------------------------------------|
//       7                                                                     0
//
pUSART0->US_RTOR = 0; // receiver time-out (disabled)
// USART0 transmitter TimeGuard Register US_TTGR    (read/write)
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                                                       |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//          31       30       29       28       27       26       25       24
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                                                       |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//          23       22       21       20       19       18       17       16
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                                                       |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//          15       14       13       12       11       10        9       8
//
// |-----------------------------------------------------------------------|
// |                               TG                                      |
// |-----------------------------------------------------------------------|
//       7                                                                     0
//
pUSART0->US_TTGR = 0; // transmitter timeguard (disabled)
// USART0 FI DI RatioRegister   US_FIDI    (read/write)
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                                                       |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//           31       30       29       28       27       26       25       24
//
// |--------|--------|--------|--------|--------|--------|--------|--------|
// |                                                                       |
// |--------|--------|--------|--------|--------|--------|--------|--------|
//           23       22       21       20       19       18       17       16
//
// |--------|--------|--------|--------|--------|--------------------------|
// |                                                  FI_DI_RATIO          |
// |--------|--------|--------|--------|--------|--------------------------|
//           15       14       13       12       11       10        9       8
//
// |-----------------------------------------------------------------------|
// |                           FI_DI_RATIO                                 |
// |-----------------------------------------------------------------------|
//       7                                                                     0
//  not used, nothing to set up here
// USART0 Number of Errors Register   US_NER    (read only)
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       31                                                                   24
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       23                                                                   16
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       15                                                                    8
//
// |-----------------------------------------------------------------------|
// |                           NB_ERRORS                                   |
// |-----------------------------------------------------------------------|
//        7                                                                     0
//  Read-only, nothing to set up here
// USART0 IrDA Filter Register  US_IF   (read/write)
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       31                                                                   24
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       23                                                                   16
//
// |-----------------------------------------------------------------------|
// |                                                                       |
// |-----------------------------------------------------------------------|
//       15                                                                    8
//
// |-----------------------------------------------------------------------|
// |                           IRDA_FILTER                                 |
// |-----------------------------------------------------------------------|
//        7                                                                     0
//  not used, nothing to set up here
// Set up the Advanced Interrupt Controller (AIC)  registers for USART0
volatile AT91PS_AIC pAIC = AT91C_BASE_AIC; // pointer to AIC data structure
pAIC->AIC_IDCR = (1<<AT91C_ID_US0); // Disable USART0 interrupt in AIC
pAIC->AIC_SVR[AT91C_ID_US0] = // Set the USART0 IRQ handler address in AIC Source
     (unsigned int)Usart0IrqHandler;         // Vector Register[6]
pAIC->AIC_SMR[AT91C_ID_US0] = // Set the interrupt source type(level-sensitive) and
    (AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | 0x4 ); // priority (4) in AIC Source Mode Register[6]
pAIC->AIC_IECR = (1<<AT91C_ID_US0);  // Enable the USART0 interrupt in AIC
// enable the USART0 receiver and transmitter
pUSART0->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
// enable the USART0 receive interrupt
pUSART0->US_IER = AT91C_US_RXRDY; // enable RXRDY usart0 receive interrupt
pUSART0->US_IDR = ~AT91C_US_RXRDY; // disable all interrupts except RXRDY
// set up buffer pointer and character counter
pBuffer = &Buffer[0];
nChars = 0;
// enable IRQ interrupts
enableIRQ();
// at this point, only the USART0 receive interrupt is armed!
}
