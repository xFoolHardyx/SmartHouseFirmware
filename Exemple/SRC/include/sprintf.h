/*
 * sprintf.h
 *
 *  Created on: 9 זמגע. 2012
 *      Author: Trol
 */

#ifndef SPRINTF_H_
#define SPRINTF_H_

typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;

#define va_start(v,l)	__builtin_va_start(v,l)
#define va_end(v)		__builtin_va_end(v)
#define va_arg(v,l)		__builtin_va_arg(v,l)

#define putchar(c) c;


int	 sprintf	( char *buf, const char *restrict fmt, ... );

#endif /* SPRINTF_H_ */
