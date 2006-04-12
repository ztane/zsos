#include <new>
#include "freepagelist"

FreePageList free_page_list;

FreePageList::FreePageList()
{

}

void FreePageList::initialize(unsigned long nPages, void* firstFreeMemory)
{
	freePages = new (firstFreeMemory) BitVector(nPages);
	lastAvailablePage = nPages - 1;
}

