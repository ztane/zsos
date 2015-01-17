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

                MemMapArea *bss;

	public:
		MemMap() : begin(), end() {
			end.base = VirtAddr((void*)0xFFFFFFFFUL);
			end.end = VirtAddr((void*)0xFFFFFFFFUL);
			end.lower = &begin;
			begin.upper = &end;
		}
		VirtAddr findSlot(size_t size);

		int addArea(MemMapArea *a);
		int addAreaAt(MemMapArea *a);
		int removeArea(MemMapArea *a);
		void dumpMemMap() const;

		void *setBrk(void *newBrk) {
			if (! bss) {
				kernelPanic("Tried to adjust heap when no available!");
			}

                        // shrink beyond beginning?
                        // linux returns the beginning of area, so do we!
			if (VirtAddr(newBrk) < bss->base) {
				return bss->end.toPointer();
			}

			uintptr_t tmp = (uintptr_t)newBrk;
			// round up
			tmp +=  0xFFF;
			tmp &= ~0xFFF;

			bss->setEnd(VirtAddr((void*)tmp));
			return newBrk;
		}

                void setBss(MemMapArea *area) {
                        bss = area;
                }

		MemMapArea *findAreaByAddr(VirtAddr a) const {
			if (! a.inUserSpace())
				return 0;

			MemMapArea *cur = begin.upper;

			while (cur->base <= a) {
				if (cur->contains(a))
					return cur;

				cur = cur->upper;
			}
			return 0;
		}
};

#endif
