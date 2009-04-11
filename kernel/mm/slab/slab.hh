#ifndef __SLAB_HH__INCLUDED__
#define __SLAB_HH__INCLUDED__

#include <stddef.h>
#include <kernel/mm/pageframe.hh>

class SlabCache {
private:
	uint32_t slabSize;
	uint32_t usedSlabs;
	uint32_t usedPages;
	uint32_t freeSlabs;
	uint32_t allocatedPages;
	uint32_t spareSlabs;
	uint32_t slabsPerPage;
	char     name[32];

	void  _allocatePages(uint32_t numPages);
	void  _prepareAllocatedPage(PageFrame *f);
	void *_allocateSlab(PageFrame *f);
	void  _releaseSlab(PageFrame *f, void *slab);
	void  _setHead(PageFrame *f);
	void  _removeHead(PageFrame *f);

	PageFrame *cacheHead;
	void operator=(const SlabCache& another);
	SlabCache(const SlabCache& another);

public:
	SlabCache(uint32_t slabSize, uint32_t freePages = 1, char *name = "<unnamed>");

	void *allocate();
	static void release(void *);
};

class FreeSlabEntry {
private:
	int nextFree;
public:
};

#endif
