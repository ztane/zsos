#include <stdio.h>
#include <stdlib.h>
#include "multiboot.hh"

MultibootInfo::MultibootInfo() { };

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

