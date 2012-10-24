#include "include/sprintf.h"

//#include <stdio.h>

int sprintf( char *buf, const char *restrict fmt, ... )
{
	int len;
	va_list va;
	va_start( va, fmt );

	len = vsprintf( buf, fmt, va );

	va_end( va );
	return len;
}
