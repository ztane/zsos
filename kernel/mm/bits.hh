#ifndef __MM_TYPES_INCLUDED__
#define __MM_TYPES_INCLUDED__

#include <inttypes.h>

class pageaddr_t {
private:
	uint32_t addr;
public:
	pageaddr_t(uint32_t a = 0xFFFFFFFFUL) : addr(a) { };	
	
	void *toLinear() {
		return (void*)(0xC0000000UL + (addr << 12));
	}

	static const pageaddr_t fromLinear(void *a) {
		return (((intptr_t)a - 0xC0000000UL) >> 12);
	}

	bool operator<(const pageaddr_t& another) const {
		return this->addr < another.addr;
	} 

	bool operator<=(const pageaddr_t& another) const {
		return this->addr <= another.addr;
	} 

	bool operator==(const pageaddr_t& another) const {
		return this->addr == another.addr;
	} 

	operator uint32_t() const {
		return addr;
	}
};

#endif
