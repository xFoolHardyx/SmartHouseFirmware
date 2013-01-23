#ifndef TWI_H_
#define TWI_H_

#include <AT91SAM7X256.h>

#define NACK (AT91C_TWI_NACK)

#define AT91C_TWI_CLOCK 	8000
#define MCK             	47923200
#define MREAD_BIT 			12

int AT91F_TWI_ReadByte(const AT91PS_TWI pTwi, int mode, int int_address, char *data, int nb);
int AT91F_TWI_WriteByte(const AT91PS_TWI pTwi, int mode, int int_address, char *data2send, int nb);
void AT91F_TWI_Open(void);

#endif /* TWI_H_ */
