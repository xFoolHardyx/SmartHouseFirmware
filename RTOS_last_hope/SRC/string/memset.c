#include <string.h>

void* memset( void *dst, int c, size_t len )
{
	uint8 *c_dst;

	c_dst = (uint8*)dst;

	while( len-- )
		*c_dst++ = (uint8)c;
	return dst;
}
