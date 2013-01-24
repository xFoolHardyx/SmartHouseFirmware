#ifndef TWI_H_
#define TWI_H_

#include <AT91SAM7X256.h>

#define NACK (AT91C_TWI_NACK)

#define AT91C_TWI_CLOCK 	8000
#define MCK             	47923200
#define MREAD_BIT 			12
#define TWIWrite						( ( unsigned long ) 0 )
#define TWIRead							( ( unsigned long ) 1 )

void vTWIMessage(const AT91PS_TWI pTwi, unsigned char * const pucData, long lDataLength, unsigned char ucSlaveAddr, unsigned long ulDirection, int iBuffAddr);
void AT91F_TWI_Open(void);

#endif /* TWI_H_ */
