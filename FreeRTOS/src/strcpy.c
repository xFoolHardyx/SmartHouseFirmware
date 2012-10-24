#include "include/string.h"

char * strcpy( char *dst, const char *src )
{
	char *rt = dst;

	while( (*dst++ = *src++) );

	return rt;
}
