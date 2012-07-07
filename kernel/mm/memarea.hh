#ifndef __MEMAREA_HH_INCLUDED__
#define __MEMAREA_HH_INCLUDED__

#include <inttypes.h>
#include <iostream>

#include "kernel/mm/bits.hh"
#include "kernel/mm/pageframe.hh"
#include "kernel/panic.hh"
#include <kernel/interrupt.hh>

class PageAllocation {
protected:
	PageFrame *start;
	size_t     amt;

public:
	PageAllocation() {
		start = 0;
		amt = 0;
	}

	inline PageFrame *getAddress() const {
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
	PageFrame *start;
	size_t     length;

public:
	MemoryArea() {}

	void init(uintptr_t _start, size_t _length)
	{
		start  = frames.getFrameByNumber(_start);
		length = _length;
	}

	PageAllocation& allocatePages(size_t amt, PageAllocation& alloc)
	{
		PageFrame *s = start;

		bool state = disableInterrupts();
		for (size_t i = 0; i < length; i ++, s++)
		{
			for (size_t l = 0; i < length && l < amt; i++, l++)
			{
				PageFrame& f = *(s + l);

				if (! (f.flags & PageFrame::IS_RAM)
						|| f.refs > 0)
					break;

				if (l == amt - 1) {
					frames.acquireRange(s, amt);
					alloc.start = s;
					alloc.amt = amt;
					enableInterruptsIf(state);
					return alloc;
				}
			}
		}
		enableInterruptsIf(state);

		alloc.amt = 0;
		return alloc;
	}

	PageFrame *allocatePage()
	{
		bool state = disableInterrupts();
		for (PageFrame *i = start; i < (start + length); i ++)
                {
                	PageFrame& f = (*i);

                        if (! (f.flags & PageFrame::IS_RAM) || f.refs > 0) {
                                continue;
			}

	                f.acquire();
			enableInterruptsIf(state);
			return &f;
                }

		enableInterruptsIf(state);
		kernelPanic("Out of free pages in allocatePage");
		return 0;
	}

	void releasePages(PageAllocation& alloc) {
		bool state = disableInterrupts();
		frames.releaseRange(alloc.start, alloc.amt);
		alloc.amt = 0;
		enableInterruptsIf(state);
	}

	void releasePages(PageFrame *start, size_t amt) {
		bool state = disableInterrupts();
		frames.releaseRange(start, amt);
		enableInterruptsIf(state);
	}
};

extern MemoryArea    DMAMemory;
extern MemoryArea NormalMemory;
extern MemoryArea   HighMemory;

#endif
