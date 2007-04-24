#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <stdarg.h>
#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus 
extern "C" { 
#endif

int vsnprintf(char *str, size_t n, const char *fmt, va_list ap);
int snprintf(char *str, size_t n, const char *fmt, ...);
int vsprintf(char *str, const char *fmt, va_list ap);
int sprintf(char *str, const char *fmt, ...);

void *calloc(size_t nmemb, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);


#ifdef __cplusplus 
}; 
#endif

#endif
