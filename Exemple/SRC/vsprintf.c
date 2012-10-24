#include <sys/cdefs.h>
#include <sys/types.h>

#include <stdio.h>
#include "include/_nulstream.h"

static int _strn_write( struct __FILE *fd, void *buf, size_t size );

typedef struct __str_fh {
	int	bytes;	/* number of bytes written to str */
} _str_fh_t;

int vsnprintf( char *buf, size_t n, const char *restrict fmt, va_list va )
{
	FILE str;
	_str_fh_t fh;

	if( !n || !buf )
		return 0;

	fh .bytes = 0;

	str .wpos = buf;
	str .wl = n - 1;
	str .flags = FILE_W | FILE_FLUSH;
	str .rbuf .base = buf;
	str .rbuf .size = n - 1;
	str .fh = (void*) &fh;
	str .read = _nul_read;
	str	.write = _strn_write;
	str .seek = _nul_seek;
	str .close = _nul_close;
	str .flock = _MC_LOCK_INIT;

	vfprintf( &str, fmt, va );

	buf[ fh .bytes ] = '\0';

	return fh .bytes;
}

static int _strn_write( struct __FILE *fd, void *buf, size_t size )
{
	_str_fh_t *fh = (_str_fh_t*)fd ->fh;
	fh ->bytes += size;
	return -EIO;
}
