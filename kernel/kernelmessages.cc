#include <kernel/kernelmessages.hh>

#ifdef __arm__

#   include <kernel/arch/current/uart.hh>
    CONSOLE_OUTPUT_FUNCP writeToConsole = uart_write;

#else

#   include <kernel/arch/current/initial_vga.h>
    CONSOLE_OUTPUT_FUNCP writeToConsole = vga_buf_write;

#endif

extern "C" void initConsoleDriver() {
#ifdef __arm__
        uart_init();
#else
        init_vga_buffer();
#endif
}

