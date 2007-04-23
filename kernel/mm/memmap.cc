#include "kernel/mm/memmap.hh"
#include "kernel/printk.h"

#include <iostream>

int MemMap::addAreaAt(MemMapArea *a) {
	a->lower = a->upper = 0;

	if (! validBase(a->base)) {
		return -1;	
	}
	if (! validEnd(a->end)) {
		return -2;
	}
	if (! a->base.isPageGranular() || ! a->end.isPageGranular()) {
		return -3;
	}

	MemMapArea *current = &begin;
	while (current != &end) {
		if (a->base > current->base) {
			current = current->upper;
			continue;
		}
	}

	if (a->end > current->base) {
		return -4;
	}

	if (a->base < current->lower->end) {
		return -5;
	}

	current->lower->upper = a;
	a->lower = current->lower;
	a->upper = current;
	current->lower = a;

	return 0;
}

int MemMap::addArea(MemMapArea *a) {
	return 0;
}

int MemMap::removeArea(MemMapArea *a) {
	return 0;
}

void MemMap::dumpMemMap() const {
	kout << "MemMap dump\n";

	MemMapArea *cur = begin.upper;
	while (cur != &end) {
		printk("\t%08X - %08X\n", (uint32_t)cur->base,
			(uint32_t)cur->end - 1);
		cur = cur->upper;
	}	

	kout << "<< end >>\n";
}
