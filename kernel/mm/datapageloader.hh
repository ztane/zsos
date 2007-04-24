#ifndef __DATAPAGELOADER_HH_INCLUDED__
#define __DATAPAGELOADER_HH_INCLUDED__

#include "kernel/mm/pageloader.hh"

class DataPageLoader : public PageLoader {
public:
	virtual int loadPage
		(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr);

	virtual int swapIn      
		(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr);

	virtual int doCow       
		(UserTask *t, MemMapArea *m, VirtAddr addr);
	
	virtual ~DataPageLoader() { }
};

extern DataPageLoader dataPageLoader;

#endif
