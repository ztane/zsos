#ifndef __TEXTPAGELOADER_HH_INCLUDED__
#define __TEXTPAGELOADER_HH_INCLUDED__

#include "kernel/mm/pageloader.hh"

class TextPageLoader : public PageLoader {
public:
	virtual int loadPage
		(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr);

	virtual int swapIn      
		(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr);

	virtual int doCow       
		(UserTask *t, MemMapArea *m, VirtAddr addr);
	
	virtual ~TextPageLoader() { }
};

extern TextPageLoader textPageLoader;

#endif
