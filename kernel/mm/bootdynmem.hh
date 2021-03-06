#ifndef __BOOTDYNMEM_INCLUDED__
#define __BOOTDYNMEM_INCLUDED__

#include <stdlib.h>
#include <allocator>
#include <kernel/panic.hh>
#include <printk.h>

class BootDynMemAllocator : public Allocator {
	int32_t   bytes_left;
	char      *next_free;
public:
	BootDynMemAllocator(void *start, uint32_t max_bytes, uint32_t skip = 0) {
		printk("%08x", start);
		intptr_t startaddr = (intptr_t)start + skip;
		if (startaddr % 16) {
			startaddr += 15;
			startaddr &= ~0xF;
			max_bytes -= 16;
		}

		printk("%08x", startaddr);
		next_free  = ((char*)startaddr);
		bytes_left = max_bytes - skip - 16;
	}

	void *allocate(size_t size) {
		void *ptr = next_free;

		// align 8 (as per libc malloc)
		size +=  7;
		size &= ~7;

		bytes_left -= size;
		next_free  += size;
		
		if (bytes_left < 0) {
			kernelPanic("Initial boot memory pool exhausted.");
		}

		return (void*)ptr;
	}

	void release(void *ptr) {
		printk("Accessed at %08x", ptr);
		kernelPanic("Tried to release memory allocated from boot memory pool");
	}
};

extern BootDynMemAllocator boot_dynmem_alloc;

#endif
