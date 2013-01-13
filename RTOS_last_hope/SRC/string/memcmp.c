#include <string.h>

int memcmp( const void *p1, const void *p2, size_t size )
{
	char *c1 = (char*)p1;
	char *c2 = (char*)p2;
	int r = 0;

	while( size-- )
	{
		if( *c1 != *c2 )
		{
			r = *c1 - *c2;
			break;
		}

		++c1;
		++c2;
	}

	return r;
}
