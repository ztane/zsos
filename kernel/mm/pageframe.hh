#ifndef __PAGEFRAME_HH_INCLUDED__
#define __PAGEFRAME_HH_INCLUDED__

#include <cstdlib>
#include <allocator>
#include "kernel/atomic.hh"	
#include "kernel/refcount.hh"
#include "kernel/mm/bits.hh"

class MemoryArea;

class PageFrame {
private:
	Atomic     flags;
	RefCount   refs;
	RefCount   pte_refs;
	Atomic     privdata;
	pageaddr_t addr;
public:
	enum {
		IS_RAM = 1,
		LOCKED = 2,
		KERNEL = 4,
		HIGH   = 8,
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

	inline void set_flag(int32_t flag) {
		flags |= flag;
	}

	inline void clear_flag(int32_t flag) {
		flags &= ~flag;
	}

	inline int32_t get_flags() const {
		return (int32_t)flags;
	}

	inline pageaddr_t getPageAddr() const {
		return addr;
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
#endif
