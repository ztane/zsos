#include "kernel/mm/memmap.hh"

void MemMap::addArea(MemMapArea *a) {
	a->lower = a->upper = 0;

	if (! root) {
		root = a;
		return;
	}

	MemMapArea *current = root;
	while (1) {
		if (a->begin > current->begin) {
			if (current->upper) {
				current = current->upper;
				continue;
			}
			
			current->upper = a;
			a->parent = current;
			break;
		}
	}
}
void removeArea(MemMapArea *a) {
};

#endif
