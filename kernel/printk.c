#include <stdarg.h>
#include "kernelmessages.hh"
#include "stdlib.h"
#include "string.h"

void printk(const char *fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	writeToConsole(buf, strlen(buf));
}
