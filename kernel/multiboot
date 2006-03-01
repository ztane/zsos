#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

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

typedef struct _multiboot_header {
	unsigned long magic;
	unsigned long flags;
	unsigned long checksum;
	unsigned long header_addr;
	unsigned long load_addr;
	unsigned long load_end_addr;
	unsigned long bss_end_addr;
	unsigned long entry_addr;
	unsigned long mode_type;
	unsigned long width;
	unsigned long height;
	unsigned long depth;
} multiboot_header_t;

typedef struct _multiboot_mod_t {
	unsigned long mod_start;
	unsigned long mod_end;
	unsigned long string;
	unsigned long reserved;
} multiboot_mod_t;

typedef struct _multiboot_aout {
	unsigned long tabsize;
	unsigned long strsize;
	unsigned long addr;
	unsigned long reserved;
} multiboot_aout_t;

typedef struct _multiboot_elf {
	unsigned long num;
	unsigned long size;
	unsigned long addr;
	unsigned long shndx;
} multiboot_elf_t;

/* NOTE! mmap structure addresses point to base_addr_low,
 * not to size when given through multiboot_info_t. Be
 * shure to correct this before you try to use them!
 */
typedef struct _multiboot_mmap {
	unsigned long size;
	unsigned long base_addr_low;
	unsigned long base_addr_high;
	unsigned long length_low;	
	unsigned long length_high;
	unsigned long type;
} multiboot_mmap_t;

/* NOTE! last element in struct is drive_ports (unseen) and
 * its size is undefined. Drive_ports is an array of I/O ports
 * used for the drive in BIOS code. It consists of zero or more
 * unsigned two-bytes integers, and is terminated with zero.
 * Use the size field to travel through the list.
 */
typedef struct _multiboot_drives {
	unsigned long size;
	unsigned char drive_number;
	unsigned char drive_mode;
	unsigned short drive_cylinders;
	unsigned char drive_heads;
	unsigned char drive_sectors;
} multiboot_drives_t;
	
typedef struct _multiboot_apm {
	unsigned short version;
	unsigned short cseg;
	unsigned long offset;
	unsigned short cseg_16;
	unsigned short dseg;
	unsigned short flags;
	unsigned short cseg_len;
	unsigned short cseg_16_len;
	unsigned short dseg_len;
} multiboot_apm_t;

typedef struct _multiboot_info_t {
	unsigned long flags;
	unsigned long mem_lower;
	unsigned long mem_upper;
	unsigned long boot_device;
	unsigned long cmdline;
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
	unsigned long boot_loader_name;
	unsigned long apm_table;
	unsigned long vbe_control_info;
	unsigned long vbe_mode_info;
	unsigned short vbe_mode;
	unsigned short vbe_interface_seg;
	unsigned short vbe_interface_off;
	unsigned short vbe_interface_len;
} multiboot_info_t;

class MultibootInfo {
	private:
		unsigned long flags;
		unsigned long mem_lower;
		unsigned long mem_upper;
		unsigned long boot_device;
		unsigned long cmdline;
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
		unsigned long boot_loader_name;
		unsigned long apm_table;
		unsigned long vbe_control_info;
		unsigned long vbe_mode_info;
		unsigned short vbe_mode;
		unsigned short vbe_interface_seg;
		unsigned short vbe_interface_off;
		unsigned short vbe_interface_len;
	public:
		MultibootInfo();
		unsigned int	get_flags()		const;
		unsigned int	get_low_mem()		const;
		unsigned int	get_high_mem()		const;
		const char *	get_cmdline()		const;
		unsigned int	get_mmap_addr()		const;
		unsigned int	get_mmap_length()	const;
		const char *	get_loader_name()	const;
};

#endif
