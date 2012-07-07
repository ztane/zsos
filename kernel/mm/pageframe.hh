#ifndef __PAGEFRAME_HH_INCLUDED__
#define __PAGEFRAME_HH_INCLUDED__

#include <inttypes.h>
#include <cstdlib>
#include <allocator>
#include "kernel/atomic.hh"
#include "kernel/refcount.hh"
#include "kernel/mm/bits.hh"
#include "kernel/panic.hh"

class MemoryArea;
class SlabCache;

#define __SLAB_CACHE_HEAD (pte_refs)
#define __SLAB_NEXT_PAGE  (privdata)
#define __KMALLOC_SIZE    (privdata)

class PageFrame {
protected:
	Atomic     flags;
	RefCount   refs;
	RefCount   pte_refs;
	Atomic     privdata;

public:
	enum {
		IS_RAM      = 1,
		LOCKED      = 2,
		KERNEL      = 4,
		HIGH        = 8,
		COW         = 16,
		SLAB        = 32,
		KMALLOC_BIG = 64
	};

	PageFrame() : refs(), pte_refs(), privdata() {
		flags = LOCKED;
	}

	inline void acquire() {
		refs ++;
	}

	inline bool release() {
		return -- refs;
	}

	inline void setFlag(int32_t flag) {
		flags |= flag;
	}

	inline void clearFlag(int32_t flag) {
		flags &= ~flag;
	}

	inline int32_t getFlags() const {
		return (int32_t)flags;
	}

	inline uint32_t getFirstFreeSlab() {
		// get first free slab index
		if (! (flags & SLAB)) {
			kernelPanic("getFirstFreeSlab on non-slab!");
		}

		int index = (flags & 0xFF800000) >> (32 - 9);
		return index;
	}

	inline void setFirstFreeSlab(uint32_t index) {
		// get first free slab index
		if (! (flags & SLAB)) {
			kernelPanic("setFirstFreeSlab on non-slab!");
		}

		index &= 0x1FF;
		index <<= 32 - 9;
		flags &= ~ 0xFF800000;
		flags |= index;
	}

	inline uint32_t getKMallocSize() const {
		return __KMALLOC_SIZE;
	}

	inline uint32_t getFreeSlabs() const {
		return __SLAB_NEXT_PAGE & 0xFFF;
	}

	inline void setFreeSlabs(uint32_t newValue) {
		__SLAB_NEXT_PAGE = (__SLAB_NEXT_PAGE & ~0xFFF) | newValue;
	}

	inline uintptr_t getFrameNumber() const;

	void *toLinear() const;

	PageFrame *getNextPage();
	inline void setNextPage(PageFrame *_nextPage) {
		void *pageAddr = 0;

		if (_nextPage != 0) {
			 pageAddr = _nextPage->toLinear();
		}

		__SLAB_NEXT_PAGE = (__SLAB_NEXT_PAGE & 0xFFF) | ((uint32_t)pageAddr & ~0xFFF);
	}

	inline SlabCache *getSlabHead() const {
		return (SlabCache*)(uint32_t)__SLAB_CACHE_HEAD;
	}

	inline void setSlabHead(SlabCache *cacheHead) {
		__SLAB_CACHE_HEAD = (uint32_t)cacheHead;
	}

	friend class PageFrameTable;
	friend class MemoryArea;
};

class PageFrameTable {
private:
	PageFrame  *frames;
        uintptr_t  maxPage;

public:
	PageFrameTable() {
		frames = NULL;
	}

	static uintptr_t getPageNumberFromLinear(void *addr) {
                return (((intptr_t)addr - 0xC0000000UL) >> 12);
	}

	void initialize(uintptr_t _maxPage, Allocator& alloc) {
		maxPage = _maxPage;
		frames = new (alloc) PageFrame[_maxPage + 1];
	}

	inline uintptr_t getFrameNumber(PageFrame *frame) const {
		return frame - frames;
	}

	inline PageFrame *getFrameByLinear(void *linear) const {
		return getFrameByNumber(getPageNumberFromLinear(linear));
	}

	inline PageFrame *getFrameByNumber(int number) const {
		return frames + number;
	}

	inline uintptr_t getLastPageNumber() const {
		return maxPage;
	}

	void setFlagsRange  (PageFrame *start, size_t npages, int32_t flag);
	void clearFlagsRange(PageFrame *start, size_t npages, int32_t flag);
	void acquireRange   (PageFrame *start, size_t npages);
	void releaseRange   (PageFrame *start, size_t npages);

	friend class MemoryArea;
	friend class PageFrame;
};

extern PageFrameTable frames;

inline PageFrame *PageFrame::getNextPage() {
	void *addr = (void*)((uint32_t)(__SLAB_NEXT_PAGE) & ~0xFFF);

	if (addr == 0)
		return 0;

	return frames.getFrameByLinear(addr);
}

inline void* PageFrame::toLinear() const {
	uintptr_t pageNumber = this - frames.frames;
	return (void*)(0xC0000000UL + (pageNumber << 12));
}

inline uintptr_t PageFrame::getFrameNumber() const {
    return this - frames.frames;
}

#endif

