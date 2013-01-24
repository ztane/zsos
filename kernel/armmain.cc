/* main.c - the entry point for the kernel */

#include <stdint.h>

#include <kernel/arch/current/uart.h>

void *set_gfx_mode(uint32_t w, uint32_t h, uint32_t bpp);

#define UNUSED(x) (void)(x)

const char hello[] = "\r\nHello World\r\n";
const char halting[] = "\r\n*** system halting ***";

const char *to_hex = "0123456789ABCDEF";

void print_hex(uint32_t number) {
    while (number) {
        uart_putc(to_hex[number & 0xF]);
        number >>= 4;
    }
}

// kernel main function, it all begins here
extern "C" void kmain(uint32_t r0, uint32_t r1, uint32_t atags) {
    UNUSED(r0);
    UNUSED(r1);
    UNUSED(atags);
    uart_init();

    uart_puts(hello);
/*
    uint32_t width = 1024;
    uint32_t height = 768;
    uint32_t *framebuffer = (uint32_t*)set_gfx_mode(width, height, 32);
    uint32_t *pos = framebuffer;

    for (uint32_t h = 0; h < height; h ++) {
        for (uint32_t w = 0; w < width; w ++, pos ++) {
            *pos = (h >> 2 << 8) | (w >> 2);
        }
    }
    print_hex((uint32_t)framebuffer);
*/

    print_hex((uint32_t)kmain);

    while (1) {
	uart_putc(uart_getc());
	uart_putc('>');
    }

    uart_puts(halting);
}
