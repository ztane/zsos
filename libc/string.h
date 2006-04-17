#ifndef __STRING_H__
#define __STRING_H__

#ifdef __cplusplus 
extern "C" { 
#endif

#include <stdlib.h>

void *_memsetd(void *s, unsigned int f, unsigned int n);
void *memset(void *s, int c, unsigned int n);
void *memcpy(void *, const void *, size_t);
char *strncpy(char *dest, const char *src, size_t n);
void *memmove(void *, const void *, size_t);
size_t strlen(register const char *s);
int    strncmp(const char *s1, const char *s2, size_t n);

#ifdef __cplusplus 
};
#endif

#endif
