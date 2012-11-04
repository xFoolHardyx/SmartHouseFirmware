#include "include/string.h"

char* strcat( char *dst, const char *src )
{
	char *cdst = dst;

	while( *dst )
		dst++;

	while( (*dst++ = *src++) );

	return cdst;
}
