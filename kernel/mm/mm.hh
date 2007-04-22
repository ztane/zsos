#ifndef __MM_HH_INCLUDED__
#define __MM_HH_INCLUDED__

#include "kernel/mm/pageframe.hh"

class MM {
private:
	static PageFrame * volatile global0Page;
public:
	static PageFrame *allocateZeroPage();
	static PageFrame *getGlobalZeroPage();
};

#endif
