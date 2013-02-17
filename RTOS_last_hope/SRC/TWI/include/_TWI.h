#ifndef TWI_H_
#define TWI_H_

#define AT91C_TWI_CLOCK 	8000
#define MCK             	47923200
#define MREAD_BIT 			12

#define TWIWrite						( ( unsigned long ) 0 )
#define TWIRead							( ( unsigned long ) 1 )

void vTWIMessage(unsigned char * const pucData, long lDataLength, unsigned char ucSlaveAddr, int iBuffAddr, unsigned long ulDirection);
void vTWIInit(void);

#endif /* TWI_H_ */
