#ifndef VIRTADDR_HH_INCLUDED
#define VIRTADDR_HH_INCLUDED

#include "kernel/mm/bits.hh"
#include <inttypes.h>

class VirtAddr {
private:
	uint32_t address;
public:
	explicit VirtAddr(uint32_t a = 0) : address(a) { }
	explicit VirtAddr(void *a) : address((uint32_t)a) { }
	
	static const VirtAddr fromPageAddr(pageaddr_t p) {
		return VirtAddr((p << 12) + 0xC0000000UL);
	}

        bool inUserSpace() const {
		return address < 0xC0000000UL;
	}
	
	operator uint32_t() const { return address; }
};

#endif
