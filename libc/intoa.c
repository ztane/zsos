#include "stdlib.h"
#include "unstdlib.h"
#include "wchar.h"

#ifdef TEST
	#include <limits.h>
#endif

#define MAX_STR_SIZE (sizeof(int) * 8 + 2)

static char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int itostr(char *buf, size_t n, int radix, int i)
{
	char tmp[MAX_STR_SIZE];
	unsigned int l;
	int pos = 0;

	if (n <= 0)
		return -1;

	if (n == 1) {
		*buf = 0;
		return -1;
	}

	if (radix < 2 || radix > 36)
	{
		return 0;
	}

	l = i;

	// negative: prepend - to output buffer,
	// negate to unsigned int and decrease max
	// chars...
	if (i < 0)
	{
		*(buf++) = '-';
		l = -i;
		n--;
	}

	// do-while: ensure this is run once for 0.
	do {
		tmp[pos] = digits[l % radix];
		pos ++;
		l /= radix;
	} while (l);

	// pos is off by one
	pos --;

	// fill in reverse order:
	while (n > 1 && pos >= 0) {
		*(buf++) = tmp[pos--];
		n--;
	}

	*buf = '\0';
	if (pos >= 0)
		return -1;

	return 0;
}

int uitostr(char *buf, size_t n, int radix, unsigned int l)
{
	char tmp[MAX_STR_SIZE];
	int pos = 0;

	if (n <= 0)
		return -1;

	if (n == 1) {
		*buf = 0;
		return -1;
	}

	if (radix < 2 || radix > 36)
	{
		return 0;
	}

	// do-while: ensure this is run once for 0.
	do {
		tmp[pos] = digits[l % radix];
		pos ++;
		l /= radix;
	} while (l);

	// pos is off by one
	pos --;

	// fill in reverse order:
	while (n > 1 && pos >= 0) {
		*(buf++) = tmp[pos--];
		n--;
	}

	*buf = '\0';
	if (pos >= 0)
		return -1;

	return 0;
}

int itowstr(wchar_t *buf, size_t n, int radix, int i)
{
	// notice: default char can be signed!
	// for proper conversions these must be unsigned!
	unsigned char tmp[MAX_STR_SIZE];
	int rv;

	if (n <= 0) {
		return 0;
	}

        tmp[0] = 0;
	rv = itostr((char *)tmp, n, radix, i);

	unsigned char *ptr = tmp;
	while (*ptr) {
		*buf ++ = *ptr ++;
	}
	*buf = 0;
	return rv;
}

int uitowstr(wchar_t *buf, size_t n, int radix, unsigned int i)
{
	// notice: default char can be signed!
	// for proper conversions these must be unsigned!
	unsigned char tmp[MAX_STR_SIZE];
	unsigned char *ptr = tmp;
	int rv;

	if (n <= 0) {
		return 0;
	}

        tmp[0] = 0;
	rv = uitostr((char *)tmp, n, radix, i);

	while (*ptr) {
		*buf ++ = *ptr ++;
	}
	*buf = 0;
	return rv;
}


////
// Shortcuts

int intoa(char *buf, size_t n, int i)
{
	return itostr(buf, n, 10, i);
}

int uintoa(char *buf, size_t n, int i)
{
	return uitostr(buf, n, 10, i);
}

int intoxa(char *buf, size_t n, unsigned int i)
{
	return itostr(buf, n, 16, i);
}

int uintoxa(char *buf, size_t n, unsigned int i)
{
	return uitostr(buf, n, 16, i);
}

////
// Wide char versions

int intowcs(wchar_t *buf, size_t n, int i)
{
	return itowstr(buf, n, 10, i);
}

int uintowcs(wchar_t *buf, size_t n, int i)
{
	return uitowstr(buf, n, 10, i);
}

int intoxwcs(wchar_t *buf, size_t n, unsigned int i)
{
	return itowstr(buf, n, 16, i);
}

int uintoxwcs(wchar_t *buf, size_t n, unsigned int i)
{
	return uitowstr(buf, n, 16, i);
}


