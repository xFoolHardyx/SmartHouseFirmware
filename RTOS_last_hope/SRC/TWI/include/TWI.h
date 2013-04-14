#ifndef TWI_H_
#define TWI_H_

#include <semphr.h>
#include <AT91SAM7X256.h>

typedef struct TWI_Message_for_Send
{
	unsigned char * pucBuf;
	unsigned char ucDirection;
	long lMessageLength;
	unsigned char ucInternalSizeAddr;
	unsigned uInternalAddr;

} xTWIMessage;



void TWIInit( void );

#endif
