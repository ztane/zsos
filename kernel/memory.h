#ifndef _MEMORY_H 
#define _MEMORY_H 

typedef struct _gdt_descriptor {
	unsigned char limit_7_0;
	unsigned char limit_15_8;

	unsigned char base_7_0;
	unsigned char base_15_8;
	unsigned char base_23_16;
	
	unsigned char access;
	unsigned char flags;
	unsigned char base_31_24;	
} gdt_descriptor;

typedef struct _gdt_info {
	unsigned int limit;
	unsigned int base;
	unsigned int access;
	unsigned int flags;
} gdt_info;

void get_gd_values(gdt_descriptor *desc, gdt_info *info);
void set_gd_values(gdt_info *info, gdt_descriptor *desc);

#endif
