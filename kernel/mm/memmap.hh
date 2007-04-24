#ifndef MEMMAP_HH_INCLUDED
#define MEMMAP_HH_INCLUDED

#include "kernel/mm/memmaparea.hh"
#include "kernel/panic.hh"
#include <iostream>

class MemMap {
	protected:
		MemMapArea begin;
		MemMapArea end;
		
		bool validBase(VirtAddr a) {
			return a.inUserSpace();
		}
		bool validEnd(VirtAddr a) {
			return a.inUserSpaceEx();
		}

		void *current_brk;
		void *heap_start;
	public:
		MemMap() : begin(), end() { 
			end.base = VirtAddr((void*)0xFFFFFFFFUL); 
			end.end = VirtAddr((void*)0xFFFFFFFFUL);
			end.lower = &begin;
			begin.upper = &end;
			current_brk = 0;
			heap_start = 0;
		}
		int addArea(MemMapArea *a);
		int addAreaAt(MemMapArea *a);
		int removeArea(MemMapArea *a);
		void dumpMemMap() const;
		
		void *setBrk(void *newBrk) {
			if (newBrk < heap_start) {
				return current_brk;
			}
			
			MemMapArea *a = findAreaByAddr(VirtAddr(heap_start));
			if (! a) {
				kernelPanic("Tried to adjust heap when no available!");
			}

			uintptr_t tmp = (uintptr_t)newBrk;
			// round up
			tmp +=  0xFFF;
			tmp &= ~0xFFF;
			a->setEnd(VirtAddr((void*)tmp));
			current_brk = newBrk;
			return newBrk;
		}

		MemMapArea *findAreaByAddr(VirtAddr a) const {
			kout << a.inUserSpace() << endl;
			if (! a.inUserSpace()) 
				return 0;

			MemMapArea *cur = begin.upper;

			while (cur->base <= a) {
				kout << cur->base << " " 
				     << a         << " "
				     << cur->end  << "\n";
				if (cur->contains(a))
					return cur;
				cur = cur->upper;
			}
			return 0;
		}
};

#endif
