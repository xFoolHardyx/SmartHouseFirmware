#ifndef TWI_CREATE_TEST_H_
#define TWI_CREATE_TEST_H_

#include <TWI.h>

#define NULL ((void *)0)
#define htonl(A) ((((A) & 0xff000000) >> 24) | (((A) & 0x00ff0000) >> 8) | (((A) & 0x0000ff00) << 8) | (((A) & 0x000000ff) << 24))

void vTCPHardReset( void );
long lTCPSoftReset( void );
long lTCPCreateSocket( void );
void lProcessConnection( void );
void vTCPListen( void );

#endif /* TWI_CREATE_TEST_H_ */
