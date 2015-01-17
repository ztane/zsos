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
		if (a->base < current->base) {
			break;
		}
		current = current->upper;
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

VirtAddr MemMap::findSlot(size_t size) {
	MemMapArea *current = &end;
	VirtAddr userEnd = VirtAddr::upperUserLimit();

	if (current->lower == &begin) {
		return (userEnd - size).getPageStart();
	}

	// Find the highest available slot
	while (current != &begin) {
		VirtAddr base = current->base;
		// ensure we're not trying to map in kernel :D
		if (base >= userEnd) {
			base = userEnd;
		}
		uint32_t diff = current->lower->end - base;
		if (diff >= size) {
			return (base - size).getPageStart();
		}
		current = current->lower;
	}

	return VirtAddr(0);
}

int MemMap::addArea(MemMapArea *a) {
	size_t size = a->end - a->base;
	VirtAddr mapAddr = findSlot(size);
	if (! mapAddr) {
		return ENOMEM;
	}

	a->adjust(mapAddr);
	addAreaAt(a);
	return 0;
}

int MemMap::removeArea(MemMapArea *a) {
	return 0;
}

void MemMap::dumpMemMap() const {
	kout << "MemMap dump\n";

	MemMapArea *cur = begin.upper;
	while (cur != &end) {
		printk("\t%08X - %08X\n", cur->base.toInteger(),
			cur->end.toInteger() - 1);
		cur = cur->upper;
	}

	kout << "<< end >>\n";
}
