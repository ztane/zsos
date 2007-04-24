#ifndef __ZEROPAGELOADER_HH_INCLUDED__
#define __ZEROPAGELOADER_HH_INCLUDED__

#include "kernel/mm/pageloader.hh"

class ZeroPageLoader : public PageLoader {
public:
	virtual int loadPage
		(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr);

	virtual int swapIn      
		(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr);

	virtual int doCow       
		(UserTask *t, MemMapArea *m, VirtAddr addr);
	
	virtual ~ZeroPageLoader() { }
};

extern ZeroPageLoader zeroPageLoader;

#endif
