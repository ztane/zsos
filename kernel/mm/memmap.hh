#ifndef MEMMAP_HH_INCLUDED
#define MEMMAP_HH_INCLUDED

#include "kernel/mm/memmaparea.hh"
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
	public:
		MemMap() : begin(), end() { 
			end.base = VirtAddr((void*)0xFFFFFFFFUL); 
			end.end = VirtAddr((void*)0xFFFFFFFFUL);
			end.lower = &begin;
			begin.upper = &end;
		}
		int addArea(MemMapArea *a);
		int addAreaAt(MemMapArea *a);
		int removeArea(MemMapArea *a);
		void dumpMemMap() const;
		
		MemMapArea *findAreaByAddr(VirtAddr a) const {
			kout << a.inUserSpace() << endl;
			if (! a.inUserSpace()) 
				return 0;

			MemMapArea *cur = begin.upper;

			while (cur->base < a) {
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
