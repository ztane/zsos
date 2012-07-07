#include <kernel/mm/slab/slab.hh>
#include <kernel/mm/memarea.hh>
#include <string.h>
#include "printk.h"

SlabCache::SlabCache(uint32_t _slabSize, uint32_t _sparePages, char *_name) {
	// round to nearest multiple of 16

	strncpy(name, _name, 32);
        slabSize = (_slabSize + 15) / 16 * 16;

	usedSlabs = usedPages = allocatedPages = 0;
	slabsPerPage = 4096 / slabSize;
        spareSlabs = _sparePages * slabsPerPage;

	cacheHead = 0;
	_allocatePages(_sparePages);
}

void SlabCache::_prepareAllocatedPage(PageFrame *f) {
	char *ptr = (char*)f->toLinear();
	for (uint32_t cnt = 0; cnt < slabsPerPage; cnt ++) {
		// initialize pointers...
		*(uint32_t*)ptr = cnt + 1;
		ptr += slabSize;
	}

	f->setNextPage(0);
	f->setFreeSlabs(slabsPerPage);
	f->setFirstFreeSlab(0);
	f->setSlabHead(this);
}

void SlabCache::_allocatePages(uint32_t n) {
	for (uint32_t i = 0; i < n; i++) {
		PageFrame *f = NormalMemory.allocatePage();
		f->setFlag(PageFrame::SLAB);
		f->setFirstFreeSlab(0);
		_prepareAllocatedPage(f);
		_setHead(f);
	}
}

void *SlabCache::_allocateSlab(PageFrame *f) {
	bool state = disableInterrupts();

	uint32_t freeIndex = f->getFirstFreeSlab();
	if (freeIndex >= slabsPerPage) {
		kernelPanic("first partially free slab page did not contain free slabs");
	}

	char     *address = (char*)f->toLinear();
	char     *slab    = address + freeIndex * slabSize;
	uint32_t  nextFree = *(uint32_t*)slab;

	f->setFirstFreeSlab(nextFree);

	// all used!
	if (nextFree >= slabsPerPage) {
		usedPages ++;

		_removeHead(f);
	}

	f->setFreeSlabs(f->getFreeSlabs() - 1);
	usedSlabs ++;
	freeSlabs --;

	enableInterruptsIf(state);
	return slab;
}

void SlabCache::_releaseSlab(PageFrame *f, void *slab) {
	int index = ((uint32_t)slab & 0xFFF) / slabSize;
	bool state = disableInterrupts();

	uint32_t freeSlabsOnPage = f->getFreeSlabs();
	f->setFreeSlabs(freeSlabsOnPage + 1);

	*(uint32_t *)slab = f->getFirstFreeSlab();
	f->setFirstFreeSlab(index);

	usedSlabs --;
	freeSlabs ++;

	if (freeSlabsOnPage == 0) {
		usedPages --;

		// add this to the list of free pages
		_setHead(f);
	}

	// TODO: deallocate if completely empty?
	enableInterruptsIf(state);
}

void SlabCache::_setHead(PageFrame *f) {
	PageFrame *oldHead = cacheHead;
	f->setNextPage(cacheHead);
	cacheHead = f;
}

void SlabCache::_removeHead(PageFrame *f) {
	if (f != cacheHead) {
		kernelPanic("Tried to remove wrong head from slab cache!");
	}

	cacheHead = f->getNextPage();
}

void *SlabCache::allocate() {
	bool save = disableInterrupts();

	// TODO: ensure that spare is used...
	if (cacheHead == 0) {
		_allocatePages(1);
	}

	void *rv = _allocateSlab(cacheHead);
	enableInterruptsIf(save);
	return rv;
}

void SlabCache::release(void *slab) {
	bool save = disableInterrupts();

	PageFrame *f = frames.getFrameByLinear(slab);
	SlabCache *c = f->getSlabHead();
	c->_releaseSlab(f, slab);

	enableInterruptsIf(save);
}
