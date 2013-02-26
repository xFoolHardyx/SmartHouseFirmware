#ifndef TCP_H_
#define TCP_H_

#define htonl(A) ((((A) & 0xff000000) >> 24) | (((A) & 0x00ff0000) >> 8) | (((A) & 0x0000ff00) << 8) | (((A) & 0x000000ff) << 24))

void vTCPHardReset( void );
long lTCPSoftReset( void );
long lTCPCreateSocket( void );
long lTCPListen( void );
long lProcessConnection( void );
void vTCPListen( void );

#endif /* TCP_H_ */
