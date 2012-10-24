#include "include/string.h"

int strcmp( const char *s1, const char *s2 )
{
	int rt;

	while( !(rt = *s1 - *s2) && *s1++ && *s2++ );

	return rt;
}
