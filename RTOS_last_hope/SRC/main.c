#include <settings.h>
#include <AT91SAM7X256.h>
void main (void)
{
	AT91PS_WDTC  aaa= AT91C_BASE_WDTC;
	DisWD(aaa);
	for (;;){}
}

