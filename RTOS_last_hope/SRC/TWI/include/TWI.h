#ifndef TWI_H_
#define TWI_H_

typedef struct TWI_Message_for_Send
{
	unsigned char * pucBuf;
	unsigned char 	ucDirection;
	long 			lMessageLength;
	unsigned int 	ucInternalSizeAddr;
	unsigned 		uInternalAddr;
	unsigned char	ucDevAddr;
} xTWIMessage;


void TWIInit( void );

void TWIMessage( unsigned char * pucMessage, long lMessageLength, unsigned char ucSlaveAddress, unsigned int ucCountIntAddr, unsigned uBufferAddress, unsigned char ucDirection, unsigned long ulTicksToWait);

#endif
