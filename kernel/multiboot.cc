#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <printk.h>
#include <inttypes.h>

#include "multiboot.hh"

#define MB_MMAP_TYPE_AVAIL  0x00000001

typedef struct _multiboot_mmap {
	uint32_t size;
	uint32_t base_addr_low;
	uint32_t base_addr_high;
	uint32_t length_low;	
	uint32_t length_high;
	uint32_t type;
} multiboot_mmap_t;

typedef struct _multiboot_module {
	uint32_t start;
	uint32_t end;
	uint32_t string;
	uint32_t reserved;
} multiboot_module_t;

struct multiboot_info_t {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	union {
		multiboot_aout_t aout;
		multiboot_elf_t elf;
	} boot_table;
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
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
        	cmdline = dup_string((const char*)from.cmdline, allocator);
        }
        else {
                cmdline = "";
        }

        if (flags & MB_FLAG_LOADER_NAME) {
                boot_loader_name = dup_string((const char*)from.boot_loader_name, allocator);
        }
        else {
                boot_loader_name = "";
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
				uint32_t new_max = mmap[i].start + (mmap[i].length - 1);
				if (max_ram_address < new_max)
					max_ram_address = new_max;

				i ++;
			}
	        }
	}

	if (flags & MB_FLAG_MODS && from.mods_count > 0) {
		mods       = new (allocator) MultibootModuleInfo[from.mods_count];
		mods_count = from.mods_count;

		multiboot_module_t *mod_table = (multiboot_module_t*)from.mods_addr;

		for (size_t i = 0; i < from.mods_count; i ++) {
			mods[i].start    = mod_table[i].start;
			mods[i].end      = mod_table[i].end;
			// mods[i].string   = dup_string((const char*)mod_table[i].string, allocator);
			mods[i].reserved = mod_table[i].reserved;
		}
	}
}
