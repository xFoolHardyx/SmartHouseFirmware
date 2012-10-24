#ifndef ___NULSTREAM_H
#define ___NULSTREAM_H

#include <stdio.h>

ssize_t	_nul_read( struct __FILE *fd, void *buf, size_t size );
ssize_t	_nul_write( struct __FILE *fd, void *buf, size_t size );

off64_t	_nul_seek( struct __FILE *fd, off64_t offset, int whence );
int		_nul_close( struct __FILE *fd );

#endif /*___NULLSTREAM_H*/
