#include <stdarg.h>
#include "chardev.h"
#include "stdlib.h"
#include "string.h"

extern char_device_iface vga;

static char buf[256];
void printk(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	vga.write(buf, strlen(buf));
}
