#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "multiboot"


MultibootInfo::MultibootInfo() { };

static char *dup_string(const char *from, Allocator& alloc)
{
        size_t siz = strlen(from) + 1;
        char *rv = new (alloc) char[siz];
        memcpy(rv, from, siz);
        return rv;
}

unsigned int MultibootInfo::get_flags() const
{
	return flags;
}

unsigned int MultibootInfo::get_low_mem() const
{
	return flags & MB_FLAG_MEM ? mem_lower : 0;
}

unsigned int MultibootInfo::get_high_mem() const
{
	return flags & MB_FLAG_MEM ? mem_upper : 0;
}

const char * MultibootInfo::get_cmdline() const
{
	return flags & MB_FLAG_CMDLINE ? 
		reinterpret_cast<const char *>(cmdline) : NULL;
}

unsigned int MultibootInfo::get_mmap_addr() const
{
	return flags & MB_FLAG_MMAP ?
		mmap_addr : 0;
}

unsigned int MultibootInfo::get_mmap_length() const
{
	return flags & MB_FLAG_MMAP ?
		mmap_length : 0;
}

const char * MultibootInfo::get_loader_name() const
{
	return flags & MB_FLAG_LOADER_NAME ? 
		reinterpret_cast<const char *>(boot_loader_name) : NULL;
}

MultibootInfo::MultibootInfo(const MultibootInfo& from, Allocator& allocator)
{
	this->flags     = from.flags;
        this->mem_lower = from.mem_lower;
        this->mem_upper = from.mem_upper;

        // todo: add others:

        if (flags & MB_FLAG_CMDLINE) {
        	this->cmdline = dup_string(from.get_cmdline(), allocator);
        }
        else {
                this->cmdline = 0;
        }

        if (flags & MB_FLAG_LOADER_NAME) {
               this->boot_loader_name = dup_string(from.get_loader_name(), allocator);
        }
        else {
               this->boot_loader_name = 0;
        }
 }


