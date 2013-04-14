#ifndef TWI_ISR_MY_H_
#define TWI_ISR_MY_H_

#define TWI_TRUE  1
#define TWI_FALSE 0

typedef struct AN_TWI_MESSAGE
{
	unsigned char TWI_TX_Ready;
	unsigned char TWI_TX_Comp;
	unsigned char TWI_RX_Ready;
} xReadyMessage;

#define AT91C_TWI_ACK 0

#endif
