#ifndef WIZNET_H_
#define WIZNET_H_

void sendset(void);
void message (unsigned char * pucSrc, unsigned int uiLength, unsigned char ucDevAddr, unsigned int uiInDevAddr, unsigned int ucSlct);

#endif /* WIZNET_H_ */
