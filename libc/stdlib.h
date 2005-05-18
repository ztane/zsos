#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <stdarg.h>
#include <stddef.h>

typedef unsigned long size_t;

#ifdef __cplusplus 
extern "C" { 
#endif

int vsnprintf(char *str, size_t n, const char *fmt, va_list ap);
int snprintf(char *str, size_t n, const char *fmt, ...);

#ifdef __cplusplus 
}; 
#endif 

#include <malloc.h>

#endif
