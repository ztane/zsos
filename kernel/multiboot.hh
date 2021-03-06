// -*- C++ -*-

#ifndef MULTIBOOT_INC
#define MULTIBOOT_INC 1

#include <mm/bootdynmem.hh>

#define MB_MAGIC_VALUE 0x2BADB002

#define MB_FLAG_MEM         0x00000001
#define MB_FLAG_BOOTDEV     0x00000002
#define MB_FLAG_CMDLINE     0x00000004
#define MB_FLAG_MODS        0x00000008
#define MB_FLAG_AOUT_TABLE  0x00000010
#define MB_FLAG_ELF_TABLE   0x00000020
#define MB_FLAG_MMAP        0x00000040
#define MB_FLAG_DRIVES      0x00000080
#define MB_FLAG_CONFIG      0x00000100
#define MB_FLAG_LOADER_NAME 0x00000200
#define MB_FLAG_APM_TABLE   0x00000400
#define MB_FLAG_GRAPHICS    0x00000800

#define MB_MMAP_TYPE_AVAIL  0x00000001

struct multiboot_aout_t {
	uint32_t tabsize;
	uint32_t strsize;
	uint32_t addr;
	uint32_t reserved;
};

struct multiboot_elf_t {
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
};

class MultibootInfo;

class MultibootModuleInfo {
	unsigned long start;
	unsigned long end;
	const char   *string;
	unsigned long reserved;

public:
	unsigned long get_base() const {
		return start;
	}

	unsigned long get_length() const {
		return end - start;
	}

	const char * get_string() const {
		return string;
	}

	friend class MultibootInfo;
};

class MultibootMMapInfo {
	unsigned long start;
	unsigned long length;

public:
	unsigned long get_base() const {
		return start;
	}

	unsigned long get_length() const {
		return length;
	}

	friend class MultibootInfo;
};

class MultibootInfo {
private:
	unsigned long flags;
	unsigned long mem_lower;
	unsigned long mem_upper;
	unsigned long boot_device;
	const char* cmdline;

	MultibootModuleInfo *mods;
	size_t               mods_count;
	
	union {
		multiboot_aout_t aout;
		multiboot_elf_t elf;
	} boot_table;

	MultibootMMapInfo *mmap;
	size_t             mmap_length;

	unsigned long drives_length;
	unsigned long drives_addr;
	unsigned long config_table;
	const char *boot_loader_name;
	unsigned long apm_table;
	unsigned long vbe_control_info;
	unsigned long vbe_mode_info;
	unsigned short vbe_mode;
	unsigned short vbe_interface_seg;
	unsigned short vbe_interface_off;
	unsigned short vbe_interface_len;

	unsigned long  max_ram_address;
public:
	MultibootInfo();
	MultibootInfo(const void *from, Allocator& allocator);

	unsigned int	get_flags()		const;
	unsigned int	get_low_mem()		const;
	unsigned int	get_high_mem()		const;

	const char *	get_cmdline()		const;
	const char *	get_loader_name()	const;

	const MultibootMMapInfo& mmap_entry(int slot)     const
	{
		return mmap[slot];
	}

	size_t number_of_mmap_entries() const 
	{
		return mmap_length;	
	}

	const MultibootModuleInfo& get_module(int slot) const
	{
		return mods[slot];
	}

	size_t number_of_modules() const
	{
		return flags & MB_FLAG_MODS ? mods_count : 0;
	}

	unsigned int get_max_ram_address() const;
};

#endif
