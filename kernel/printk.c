#include <stdarg.h>
#include "initial_vga.h"
#include "stdlib.h"
#include "string.h"

static char buf[256];
void printk(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	vga_buf_write(buf, strlen(buf));
}
