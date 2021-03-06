#ifndef __FREEPAGELIST_INCLUDED__
#define __FREEPAGELIST_INCLUDED__

#include <kernel/arch/current/constants.hh>
#include <bitvector>

class FreePageList {
    unsigned long lastAvailablePage;
    BitVector *free_pages;
public:
    FreePageList();
    void initialize(const MultibootInfo& bootInfo, Allocator& allocator);
};

extern FreePageList free_page_list;


#endif
