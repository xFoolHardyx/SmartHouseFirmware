#ifndef STRING_H_
#define STRING_H_


typedef unsigned char uint8;
typedef __SIZE_TYPE__ size_t;


void* 	memcpy	( void *dst, const void *src, unsigned len );
int 	memcmp	( const void *p1, const void *p2, size_t size );
void* 	memset	( void *dst, int c, size_t len );
char*   strcat  ( char *dst, const char *src );
int 	strcmp	( const char *s1, const char *s2 );
char*	strcpy	( char *dst, const char *src );
size_t	strlen	( const char *str );
char * 	strncpy	( char *dst, const char *src, size_t n );


#endif /* STRING_H_ */
