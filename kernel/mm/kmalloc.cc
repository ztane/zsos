#include <stdlib.h>
#include <string.h>
#include "kmalloc.h"
#include <kernel/printk.h>
#include <panic.hh>
#include <kernel/mm/slab/slab.hh>
#include <kernel/mm/memarea.hh>
#include <kernel/mm/pageframe.hh>
#include <iostream>

static const int N_KMALLOC_SLABS = 8;
static const int MAX_PAGE_ORDER  = 5;
static bool kmallocInitialized = false;

SlabCache *kmallocSlabs[N_KMALLOC_SLABS];

/**
 * for <= 0x80000000  returns 27 and
 * for       >= 0x10  returns 0
 */
static inline int _get_first_bit_set(size_t value)
{
	value --;
	value >>= 4;
	int bit = 0;

	while (value) {
		value >>= 1;
		bit ++;
	}
	return bit;
}

/**
 * Allocate a block of memory of given size. Return a pointer
 * to start of usable block.
 *
 * @param size_t size
 * @return void pointer to start of block
 */
void *kmalloc(size_t size)
{
	if (!size)
		return NULL;

	if (! kmallocInitialized) {
		kernelPanic("FATAL: kmalloc not yet initialized\n");
        }

	int first_bit = _get_first_bit_set(size);
	if (first_bit >= N_KMALLOC_SLABS) {
		int pageOrder = first_bit - 9;
		if (pageOrder > MAX_PAGE_ORDER) {
			printk("kmalloc was unable to allocate a block of %d bytes", size);
			kernelPanic("Kernel halted");
		}

		int pagesToAllocate = 1 << first_bit;
                PageAllocation allocation;

		NormalMemory.allocatePages(pagesToAllocate, allocation);
                PageFrame *frame = allocation.getAddress();
		frame->setFlag(PageFrame::KMALLOC_BIG);

		return frame->toLinear();
	}

	return kmallocSlabs[first_bit]->allocate();
}

/**
 * Allocate n number of elements of size size.
 * The memory is set to 0. Returns NULL on failure.
 */
void *kcalloc(size_t n, size_t size)
{
	void *rval = NULL;

	if (!size || !n) {
		return NULL;
        }

	size *= n;
	rval = kmalloc(size);

	if (!rval) {
		return NULL;
        }

	memset(rval, 0, size);
	return rval;
}

/**
 * Resize allocated memory pointed by ptr to size bytes.
 * If ptr is NULL, then krealloc acts like kmalloc(size).
 * If size is 0, then krealloc acts like kfree(ptr) and
 * returns NULL.
 * Normally krealloc returns a pointer to new memory or
 * NULL on failure. Original data is unmodified and new
 * memory isn't initialized.
 */
void *krealloc(void *ptr, size_t size)
{
	kernelPanic("krealloc not implemented");
}

/**
 * Free the memory reserved through ptr. Don't fail if user is
 * trying to release a NULL pointer. This will fail though if
 * given invalid pointer - it will cause undefined behaviour.
 *
 * @param void *ptr
 * @return none
 */
void kfree(void *ptr)
{
	if (ptr != NULL) {
                PageFrame* frame = frames.getFrameByLinear(ptr);
		uint32_t flags = frame->getFlags();
		if (flags & PageFrame::SLAB) {
			SlabCache::release(ptr);
		}
		else if (flags & PageFrame::KMALLOC_BIG) {
			size_t size = frame->getKMallocSize();
			frame->clearFlag(PageFrame::KMALLOC_BIG);
			NormalMemory.releasePages(frame, size);
		}
		else {
		        kernelPanic("tried to kfree memory not kmalloced");
		}
	}
}



extern void __set_default_allocator(Allocator *new_def);

#include <allocator>

class KMallocAllocator : public Allocator {
public:
        virtual ~KMallocAllocator() { };

        virtual void *allocate(size_t size) {
		return kmalloc(size);
	};

        virtual void release(void *ptr) {
		kfree(ptr);
	};
};

/**
 * Initialize the heap to contain basic structures.
 * returns -1 on failure, 0 on success
 *
 * NOTE: The structure of the heap should always be so
 * that there is a used block after a free block. At
 * the moment the code tries to honour this, but if you
 * add something, then remember to honour this behaviour!
 */
int kmallocInit()
{
	kmallocInitialized = true;
	printk("here1");
	new int[16];
	printk("here1");
	new SlabCache(16,      1, "kmalloc-16");
	printk("here1");
	kmallocSlabs[0]  = new SlabCache(16,      1, "kmalloc-16");
	printk("here1");
	kmallocSlabs[1]  = new SlabCache(32,      1, "kmalloc-32");
	printk("here1");
	kmallocSlabs[2]  = new SlabCache(64,      1, "kmalloc-64");
	printk("here1");
	kmallocSlabs[3]  = new SlabCache(128,     1, "kmalloc-128");
	printk("here1");
	kmallocSlabs[4]  = new SlabCache(256,     1, "kmalloc-256");
	printk("here1");
	kmallocSlabs[5]  = new SlabCache(512,     1, "kmalloc-512");
	printk("here1");
	kmallocSlabs[6]  = new SlabCache(1024,    1, "kmalloc-1024");
	printk("here1");
	kmallocSlabs[7]  = new SlabCache(2 << 11, 1, "kmalloc-2048");

	printk("here2");
	__set_default_allocator(new KMallocAllocator());
	printk("here3");
	return 1;
}
