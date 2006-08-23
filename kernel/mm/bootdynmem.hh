#ifndef __BOOTDYNMEM_INCLUDED__
#define __BOOTDYNMEM_INCLUDED__

#include <stdlib.h>
#include <allocator>
#include <kernel/panic.hh>
#include <printk.h>

class BootDynMemAllocator : public Allocator {
	int   bytes_left;
	char *next_free;
public:
	BootDynMemAllocator(void *start, int max_bytes) {
		next_free  = (char*)start;
		bytes_left = max_bytes;
	}

	void *allocate(size_t size) {
		void *ptr = next_free;

		// align 8 (as per libc malloc)
		size +=  7;
		size &= ~7;

		bytes_left -= size;
		next_free  += size;
		
		if (bytes_left < 0) {
			kernel_panic("Initial boot memory pool exhausted.");
		}

		return (void*)ptr;
	}

	void release(void *ptr) {
	}
};

extern BootDynMemAllocator boot_dynmem_alloc;

#endif
