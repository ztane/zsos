#ifndef __PAGEFRAME_HH_INCLUDED__
#define __PAGEFRAME_HH_INCLUDED__

#include <cstdlib>
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
	PageFrame() {

	}

	inline void acquire() {
		refs ++;
	}

	inline bool release() {
		return -- refs;
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

	void initialize(pageaddr_t max_addr) {
		max_page = max_addr;
		page_frames = new PageFrame[max_addr + 1];
	}

	inline PageFrame& get_frame_entry(pageaddr_t a) {
		if (a <= max_page) {
			return page_frames[a];
		}
		
		// a bug?
		return page_frames[0];
	}
};

#endif
