#ifndef __MEMAREA_HH_INCLUDED__
#define __MEMAREA_HH_INCLUDED__

#include <inttypes.h>
#include <iostream>

#include "kernel/mm/bits.hh"
#include "kernel/mm/pageframe.hh"
#include "kernel/panic.hh"

class PageAllocation {
protected:
	pageaddr_t start;
	size_t     amt;

public:
	PageAllocation() {
		start = amt = 0;
	}

	inline pageaddr_t getAddress() const {
		return start;
	}

	inline size_t getLength() const {
		return amt;
	}

	inline operator bool() {
		return amt > 0;
	}

	inline bool operator !() {
		return amt <= 0;
	}

	friend class MemoryArea;

	~PageAllocation() {
		
	}
};

class MemoryArea {
protected:
	pageaddr_t start;
	size_t     length;
	
	PageFrameTable& table;

public:
	MemoryArea(PageFrameTable& tb, 
		pageaddr_t _start, size_t _length) 
			: table(tb) 
	{
		start  = _start;
		length = _length;
		// printk("Memory area: %d %d\n", start, length);
	}

	PageAllocation& allocatePages(size_t amt, PageAllocation& alloc) 
	{
		pageaddr_t s;

		for (size_t i = 0; i < length; i ++) 
		{
			s = start + i;
			for (size_t l = 0; i < length && l < amt; i++, l++) 
			{
				PageFrame& f = table.page_frames[s + l];

				if (! (f.flags & PageFrame::IS_RAM)
						|| f.refs > 0)
					break;

				if (l == amt - 1) {
					table.acquireRange(s, amt);
					alloc.start = s;
					alloc.amt = amt;
					return alloc;
				}
			}
		}

		alloc.amt = 0;
		return alloc;	
	}

	PageFrame *allocatePage() 
	{
		for (size_t i = start; i < (start + length); i ++)
                {
                	PageFrame& f = table.page_frames[i];

                        if (! (f.flags & PageFrame::IS_RAM) || f.refs > 0)
                                continue;

	                f.acquire();
			return &f;
                }

		kernelPanic("Out of free pages in allocatePage");
		return 0;
	}

	void releasePages(PageAllocation& alloc) {
		table.releaseRange(alloc.start, alloc.amt);
		alloc.amt = 0;
	}
};

extern MemoryArea    DMAMemory;
extern MemoryArea NormalMemory;
extern MemoryArea   HighMemory;

#endif
