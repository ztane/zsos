#ifndef __ATAGS_HH_INCLUDED__
#define __ATAGS_HH_INCLUDED__

#include <inttypes.h>

const uint32_t ATAG_NONE	= 0x00000000;
const uint32_t ATAG_CORE	= 0x54410001;
const uint32_t ATAG_MEM	        = 0x54410002;
const uint32_t ATAG_VIDEOTEXT	= 0x54410003;
const uint32_t ATAG_RAMDISK	= 0x54410004;
const uint32_t ATAG_INITRD2	= 0x54420005;
const uint32_t ATAG_SERIAL	= 0x54410006;
const uint32_t ATAG_REVISION	= 0x54410007;
const uint32_t ATAG_VIDEOLFB	= 0x54410008;
const uint32_t ATAG_CMDLINE	= 0x54410009;

void atag_scanner(void *start_address);

#endif // __ATAGS_HH_INCLUDED__

