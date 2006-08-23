#include <new>
#include <multiboot.hh>
#include <allocator>
#include <mm/freepagelist.hh>


FreePageList free_page_list;

FreePageList::FreePageList()
{

}

void FreePageList::initialize(const MultibootInfo& boot_info, 
	Allocator& allocator)
{
	unsigned long num_pages = boot_info.get_max_ram_address() / 0x1000 + 1;
	
	free_pages = new (allocator) BitVector(num_pages, allocator);
}

