#ifndef __PAGEFRAME_HH_INCLUDED__
#define __PAGEFRAME_HH_INCLUDED__

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
private:
	Atomic     flags;
	RefCount   refs;
	RefCount   pte_refs;
	Atomic     privdata;
	pageaddr_t addr;

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

	inline int32_t get_flags() const {
		return (int32_t)flags;
	}

	inline pageaddr_t getPageAddr() const {
		return addr;
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

	PageFrame *getNextPage();

	inline void setNextPage(PageFrame *_nextPage) {
		void *pageAddr = 0;

		if (_nextPage != 0)
			 pageAddr = _nextPage->getPageAddr().toLinear();

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
	PageFrame  *page_frames;
	pageaddr_t  max_page;

public:
	PageFrameTable() {
		page_frames = NULL;
	}

	void initialize(pageaddr_t max_addr, Allocator& alloc) {
		max_page = max_addr;
		page_frames = new (alloc) PageFrame[max_addr + 1];
		for (uint32_t i = 0; i < max_addr; i ++) {
			page_frames[i].addr = i;
		}
	}

	inline PageFrame& getByFrame(pageaddr_t a) const {
		if (a <= max_page) {
			return page_frames[a];
		}

		// a bug?
		return page_frames[0];
	}

	inline PageFrame& getByLinear(void *linear) const {
		return getByFrame(pageaddr_t::fromLinear(linear));
	}

	inline pageaddr_t getLastPage() const {
		return max_page;
	}

	void setFlagsRange(pageaddr_t start, size_t npages, int32_t flag);
	void clearFlagsRange(pageaddr_t start, size_t npages, int32_t flag);
	void acquireRange(pageaddr_t start, size_t npages);
	void releaseRange(pageaddr_t start, size_t npages);

	friend class MemoryArea;
};

extern PageFrameTable page_frames;

inline PageFrame *PageFrame::getNextPage() {
	void *addr = (void*)((uint32_t)(__SLAB_NEXT_PAGE) & ~0xFFF);

	if (addr == 0)
		return 0;

	return &(page_frames.getByLinear(addr));
}

#endif
