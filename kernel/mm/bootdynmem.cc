#include "bootdynmem"

// defined in linker script!
extern char _BOOT_HEAP_START;
extern char _BOOT_HEAP_END;

BootDynMemAllocator boot_dynmem_alloc
	(&_BOOT_HEAP_START, &_BOOT_HEAP_END - &_BOOT_HEAP_START);
