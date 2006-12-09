#ifndef __PAGEFRAME_HH_INCLUDED__
#define __PAGEFRAME_HH_INCLUDED__

#include <cstdlib>
#include <allocator>
#include "kernel/atomic.hh"
#include "kernel/refcount.hh"

typedef size_t pageaddr_t;

class PageFrame {
private:
	Atomic   flags;
	RefCount refs;
	RefCount pte_refs;
	Atomic   privdata;
public:
	enum {
		IS_RAM = 1,
		LOCKED = 2,
		KERNEL = 4,
	};

	PageFrame() {
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

	friend class PageFrameTable;
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
	}

	inline PageFrame& get_frame_entry(pageaddr_t a) {
		if (a <= max_page) {
			return page_frames[a];
		}
		
		// a bug?
		return page_frames[0];
	}
	
	inline pageaddr_t get_last_page() {
		return max_page;
	}

	void set_flags_range(pageaddr_t start, size_t npages, int32_t flag);
	void clear_flags_range(pageaddr_t start, size_t npages, int32_t flag);
	void acquire_range(pageaddr_t start, size_t npages);
	void release_range(pageaddr_t start, size_t npages);
};

extern PageFrameTable global_page_frame_table;
#endif