#ifndef __PAGELOADER_HH_INCLUDED__
#define __PAGELOADER_HH_INCLUDED__

#include "kernel/mm/pageframe.hh"
#include "kernel/mm/memmaparea.hh"
#include "kernel/usertask.hh"

class PageLoader {
public:
	virtual int loadPage
		(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr) = 0;

	virtual int swapIn
		(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr) = 0;

	virtual int doCow
		(UserTask *t, MemMapArea *m, VirtAddr addr) = 0;

	virtual ~PageLoader() { }
};

#endif
