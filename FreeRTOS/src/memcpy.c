#include "include/string.h"

void* memcpy( void *dst, const void *src, unsigned len )
{
	void *ad = dst;
	char *c_dst, *c_src;

	c_dst = (char*)dst;
	c_src = (char*)src;

	if( c_src <= c_dst && c_src + len - 1 >= c_dst )
	{
		c_dst += len - 1;
		c_src += len - 1;

		while( len-- )
			*c_dst-- = *c_src--;
	}
	else
	{
		while( len-- )
			*c_dst++ = *c_src++;
	}
	return ad;
}
