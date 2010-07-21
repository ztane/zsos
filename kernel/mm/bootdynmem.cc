#include "bootdynmem.hh"

// defined in linker script!
extern char _BOOT_HEAP_START;
extern char _BOOT_HEAP_END;

extern uint32_t _boot_heap_bytes_consumed;

BootDynMemAllocator boot_dynmem_alloc
	(&_BOOT_HEAP_START, &_BOOT_HEAP_END - &_BOOT_HEAP_START, *&_boot_heap_bytes_consumed);
