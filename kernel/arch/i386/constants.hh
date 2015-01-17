#ifndef __ARCH_CONSTANTS_INCLUDED__
#define __ARCH_CONSTANTS_INCLUDED__

namespace Paging {
	const uint32_t PAGE_SIZE         = 4096;
	const uint32_t PAGE_ADDRESS_MASK = 0xFFFFF000;
	const uint32_t PAGE_OFFSET_MASK  = 0x00000FFF;
	const uint32_t PAGE_SHIFT        = 12;
}

#endif // __ARCH_CONSTANTS_INCLUDED__
