#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <printk.h>

#include "multiboot.hh"

#define MB_MMAP_TYPE_AVAIL  0x00000001

typedef struct _multiboot_mmap {
	unsigned long size;
	unsigned long base_addr_low;
	unsigned long base_addr_high;
	unsigned long length_low;	
	unsigned long length_high;
	unsigned long type;
} multiboot_mmap_t;

struct multiboot_info_t {
	unsigned long flags;
	unsigned long mem_lower;
	unsigned long mem_upper;
	unsigned long boot_device;
	const char* cmdline;
	unsigned long mods_count;
	unsigned long mods_addr;
	union {
		multiboot_aout_t aout;
		multiboot_elf_t elf;
	} boot_table;
	unsigned long mmap_length;
	unsigned long mmap_addr;
	unsigned long drives_length;
	unsigned long drives_addr;
	unsigned long config_table;
	const char* boot_loader_name;
	unsigned long apm_table;
	unsigned long vbe_control_info;
	unsigned long vbe_mode_info;
	unsigned short vbe_mode;
	unsigned short vbe_interface_seg;
	unsigned short vbe_interface_off;
	unsigned short vbe_interface_len;
};


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

const char * MultibootInfo::get_loader_name() const
{
	return flags & MB_FLAG_LOADER_NAME ? 
		reinterpret_cast<const char *>(boot_loader_name) : NULL;
}

unsigned int MultibootInfo::get_max_ram_address() const
{
	return max_ram_address;
}

MultibootInfo::MultibootInfo(const void *construct_from, Allocator& allocator)
{
	const multiboot_info_t& from = *(const multiboot_info_t *)construct_from;

	flags     = from.flags;
        mem_lower = from.mem_lower;
        mem_upper = from.mem_upper;

	max_ram_address = 0;

        // todo: add others:

        if (flags & MB_FLAG_CMDLINE) {
        	cmdline = dup_string(from.cmdline, allocator);
        }
        else {
                cmdline = 0;
        }

        if (flags & MB_FLAG_LOADER_NAME) {
                boot_loader_name = dup_string(from.boot_loader_name, allocator);
        }
        else {
                boot_loader_name = 0;
        }

	mmap	    = 0;
	mmap_length = 0;

	if (flags & MB_FLAG_MMAP) {
		char *mmap_old  = (char *)from.mmap_addr;
		char *mmap_save = mmap_old;
		char *mmap_end  = mmap_old + from.mmap_length;

	        for (; mmap_old < mmap_end;
        	       mmap_old = mmap_old + ((multiboot_mmap_t*)mmap_old)->size + 
		       sizeof(((multiboot_mmap_t*)mmap_old)->size))
	        {
			if (((multiboot_mmap_t*)mmap_old)->type == MB_MMAP_TYPE_AVAIL) 
			{
				mmap_length ++;
			}
	        }

		mmap = new (allocator) MultibootMMapInfo[mmap_length];

		mmap_old = mmap_save;
	        for (size_t i = 0; i < mmap_length;
        	       mmap_old = mmap_old + ((multiboot_mmap_t*)mmap_old)->size + 
		       sizeof(((multiboot_mmap_t*)mmap_old)->size))
	        {
			if (((multiboot_mmap_t*)mmap_old)->type == MB_MMAP_TYPE_AVAIL) 
			{
				mmap[i].start  = ((multiboot_mmap_t*)mmap_old)->base_addr_low;
				mmap[i].length = ((multiboot_mmap_t*)mmap_old)->length_low;
				
				// avoid overflow when 4GB RAM :)
				unsigned long new_max = mmap[i].start + (mmap[i].length - 1);
				if (max_ram_address < new_max)
					max_ram_address = new_max;

				i ++;
			}
	        }
	}
}
