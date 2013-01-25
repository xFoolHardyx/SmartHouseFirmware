#ifndef TWI_CREATE_TEST_H_
#define TWI_CREATE_TEST_H_

#include <TWI.h>

#define NULL ((void *)0)

void vTCPHardReset( void );
long lTCPSoftReset( void );
long lTCPCreateSocket( void );
long lProcessConnection( void );
void vTCPListen( void );

#endif /* TWI_CREATE_TEST_H_ */
