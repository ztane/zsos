#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>

char __printf_buffer[2048];
static void __puts(const char *buffer) {
        int len = strlen(buffer);
        write(0, buffer, len);
}


int putchar(int c) {
	char cc = c;
	write(0, &c, 1);
	return (unsigned char)c;
}

void puts(const char *buffer) {
        __puts(buffer);
        putchar('\n');
}

int printf(const char *fmt, ...) {
        int rv;
        va_list ap;
        va_start(ap, fmt);
        rv = vsnprintf(__printf_buffer, sizeof __printf_buffer, fmt, ap);
        va_end(ap);
        __puts(__printf_buffer);
        return rv;
}
