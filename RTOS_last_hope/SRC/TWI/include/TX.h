#ifndef TX_H_
#define TX_H_

#define TWI_ACK 0
#define TWI_NACK 1
#define ERROR_TWI 2
#define ERROR_TWI_OVERRUN 4

#define TWI_TRUE  1
#define TWI_FALSE 0

#define NULL ((void *)0)

void vTransmitData (void);

#endif /* TX_H_ */
