#include <new>
#include <multiboot>
#include <allocator>
#include <mm/freepagelist>


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

