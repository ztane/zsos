#ifndef VIRTADDR_HH_INCLUDED
#define VIRTADDR_HH_INCLUDED

#include "kernel/mm/bits.hh"
#include "kernel/arch/current/constants.hh"
#include <inttypes.h>

class VirtAddr {
private:
	uint32_t address;
public:
//	explicit VirtAddr(uint32_t a = 0) : address(a) { }
	explicit VirtAddr(void *a = 0) : address((uint32_t)a) { }
	
	static const VirtAddr fromPageAddr(pageaddr_t p) {
		return VirtAddr((void*)((p << 12) + 0xC0000000UL));
	}

        bool inUserSpace() const {
		return address < 0xC0000000UL;
	}
        bool inUserSpaceEx() const {
		return address <= 0xC0000000UL;
	}
	bool isPageGranular() const {
		return ! (address & Paging::PAGE_OFFSET_MASK);
	}
	
	operator uint32_t() const { return address; }
};

#endif
