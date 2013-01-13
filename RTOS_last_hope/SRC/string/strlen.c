#include <string.h>

size_t strlen( const char *str )
{
	int r = 0;

	while( *str++ )
		r++;

	return r;
}
