#ifndef TWI_H_
#define TWI_H_

#include <semphr.h>

//#include <lib_AT91SAM7S256.h>
#include <AT91SAM7X256.h>


typedef struct AN_TWI_MESSAGE
{
	long lMessageLength;					/*< How many bytes of data to send or received - excluding the buffer address. */
	unsigned char ucSlaveAddress;			/*< The slave address of the WIZnet on the I2C bus. */
	unsigned char ucBufferAddress;			/*< The address within the WIZnet device to which data should be read from / written to. */
	xSemaphoreHandle xMessageCompleteSemaphore;	/*< Contains a reference to a semaphore if the application tasks wants notifying when the message has been transacted. */
	unsigned char *pucBuffer;				/*< Pointer to the buffer from where data will be read for transmission, or into which received data will be placed. */
} xTWIMessage;

/* Constants to use as the ulDirection parameter of i2cMessage(). */
#define TWIWRITE				( ( unsigned long ) 0 )
#define TWIREAD					( ( unsigned long ) 1 )


void TWIInit( void );

#endif
