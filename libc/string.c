#include "stdlib.h"
#include "string.h"

char *strncpy(register char *dest, register const char *src, register size_t n) 
{
        char *origdest = dest;
	int orig_n = --n;

	if (n < 0) {
		return dest;
	}

        while (*src && n) {
                *dest++ = *src++;
                n--;
        }
	if (n == 0) { 
		origdest[orig_n] = 0;
	}
	else {
		*dest = 0;
	}
        return origdest;
}

size_t strlen(register const char *s)
{
        register size_t rv = 0;
        while (*s ++) rv ++;
        return rv;
}

void *_memsetd(void *s, unsigned int f, unsigned int n) 
{
        if (n == 0)
                goto end;

	// avoid wraparound :D 
	for( ; n > 0; n--) 
		((unsigned int *)s)[n - 1] = f;
end:
	return s;
}

void *memset(void *s, int c, unsigned int n) 
{
	while (((unsigned long)s & 3) && n) {
		*(char *)s++ = c;
		n --;
	}
	if (! n) {
		return s;
	}

	if (n > 4) {
		unsigned int f = c;
		f += f << 8;
		f += f << 16;
		_memsetd(s, f, n >> 2);
		s += n & ~3;
		n &= 3;
	}
		
	while (n) {
		*(char *)s++ = c;
		n --;
	}
	return s;
}

void *memcpy(register void *dest, register const void *src, register size_t n)
{
	void *origdest = dest;
	while (n)
	{
		*((char *) dest++) = *((const char *) src++);
		n--;
	}
	return origdest;
}

void *memmove(register void *dest, register const void *src, register size_t n)
{
	/* FIXME: fix this if memcpy starts to use something else
	   than chars */
	if (dest <= src || (src + n) <= dest)
		return memcpy(dest, src, n);

	src  += n - 1;
	dest += n - 1;

	while (n)
	{
		*((char *) dest--) = *((const char *) src--);
		n--;
	}
	return dest + 1;
}
