#ifndef MEMMAP_HH_INCLUDED
#define MEMMAP_HH_INCLUDED

#include "kernel/mm/memmaparea.hh"

class MemMap {
	protected:
		MemMapArea root;
	public:
		MemMap() : root(0) { }
		void addArea(MemMapArea *a);
		void removeArea(MemMapArea *a);
};

#endif
