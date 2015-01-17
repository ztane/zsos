#ifndef VIRTADDR_HH_INCLUDED
#define VIRTADDR_HH_INCLUDED

#include "kernel/mm/bits.hh"
#include "kernel/arch/current/constants.hh"
#include <inttypes.h>
#include <stdlib.h>

class VirtAddr {
private:
	uint32_t address;
public:
	static VirtAddr upperUserLimit() {
		return VirtAddr((void*)0xC0000000);
	}
	explicit VirtAddr(void *a = 0) : address((uint32_t)a) { }

        bool inUserSpace() const {
		return address < 0xC0000000UL;
	}

        bool inUserSpaceEx() const {
		return address <= 0xC0000000UL;
	}

	bool isPageGranular() const {
		return ! (address & Paging::PAGE_OFFSET_MASK);
	}

	bool operator!() const {
		return !address;
	}

	VirtAddr getPageStart() const {
		return VirtAddr((void *)(address & Paging::PAGE_ADDRESS_MASK));
	}

	VirtAddr getNextPage() const {
		return getPageStart() + Paging::PAGE_SIZE;
	}

	VirtAddr operator +(size_t adjustment) const {
		return VirtAddr((void*)(address + adjustment));
	}

	VirtAddr operator -(size_t adjustment) const {
		return VirtAddr((void*)(address - adjustment));
	}

	uint32_t operator -(const VirtAddr another) const {
		return address - another.address;
	}

        void *toPointer() const {
                return (void*)address;
        }

	uint32_t toInteger () const { return address; }

	bool operator <(const VirtAddr other) const {
		return address < other.address;
	}

	bool operator <=(const VirtAddr other) const {
		return address <= other.address;
	}

	bool operator >=(const VirtAddr other) const {
		return address >= other.address;
	}

	bool operator >(const VirtAddr other) const {
		return address > other.address;
	}

	bool operator ==(const VirtAddr other) const {
		return address == other.address;
	}
};

#endif
