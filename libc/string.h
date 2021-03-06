#ifndef __STRING_H__
#define __STRING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

void *_memsetd(void *s, unsigned int f, size_t n);
void *memset(void *s, int c, size_t n);
void *memcpy(void *, const void *, size_t);
char *strncpy(char *dest, const char *src, size_t n);
void *memmove(void *, const void *, size_t);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlen(register const char *s);
int    strncmp(const char *s1, const char *s2, size_t n);
char *strchr(const char *buf, int ch);
char *strsplit(register char **stringp, int ch);

#ifdef __cplusplus
};
#endif

#endif
