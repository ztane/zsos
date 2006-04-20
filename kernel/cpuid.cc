#include <cpuid>
#include <printk.h>
#include <iostream>
#include <string.h>

const char *flag_names[32] = {
	"fpu",	"vme",	"de",	"pse",
	"tsc",	"msr",	"pae",	"mce",
	"cxchg8","apic","<unk>","sep",
	"mtrr",	"pge",	"mca",	"cmov",
	"pat",	"pse36","psn",	"clfl",
	"<unk>","dtes",	"acpi",	"mmx",
	"fxsr",	"sse",	"sse2",	"ss",
	"htt",	"tm1",	"ia-64","pbe",
};

CPUIdentity cpu_identity;

CPUIdentity::CPUIdentity() {
	family = model = type = stepping = 0;
	_vendor_id_vals._ebx = 0;
	vendor = 0;
	flags = 0;
	max_cpuid_func = 0;	
	*processor_name = 0;
}

const char* CPUIdentity::get_flag_name(int number) {
	if (number < 0 || number > 31)
		return "<unk>";

	return flag_names[number];
}

static int is_386() {
	int rv;
	__asm__ __volatile__ (
		"pushfl;"
		"pop %0;"
		"mov %0, %%ecx;"
		"xor $0x40000, %0;"
		"push %0;"
		"popfl;"
		"pushfl;"
		"pop %0;"
		"xor %%ecx, %0;"
		"push %%ecx;"
		"popfl;"

		: "=a"(rv) : : "%ebx", "%ecx");
	return ! rv;
}

static int has_cpuid() 
{
	int rv;
	__asm__ __volatile__ (
		"pushfl;"
		"pop %0;"
		"mov %0, %%ecx;"
		"xor $0x200000, %0;"
		"push %0;"
		"popfl;"
		"pushfl;"
		"pop %0;"
		"xor %%ecx, %0;"
		"pushl %%ecx;"
		"popfl;"
		: "=a"(rv) : : "%ebx", "%ecx");
	return rv;
}

void __get_cpuid(CPUIdentity& ident)
{
	uint32_t _eax;
	uint32_t _ebx;
	uint32_t _ecx;
	uint32_t _edx;

	__asm__ __volatile__ (
		"xor %0, %0;"
		"cpuid;"
	: "=a"(_eax), "=b"(_ebx), "=c"(_ecx), "=d"(_edx));

	ident.max_cpuid_func = _eax;
	ident._vendor_id_vals._ebx = _ebx;
	ident._vendor_id_vals._edx = _edx;	
	ident._vendor_id_vals._ecx = _ecx;
	ident._vendor_id_vals.terminator = 0;

	kout << ident.vendor_string << endl;

	if (strncmp(ident.vendor_string, "AuthenticAMD", 13) == 0)
	{
		ident.vendor = 2;
	}
	else if (strncmp(ident.vendor_string, "GenuineIntel", 13) == 0) 
	{
		ident.vendor = 1;
	}

	kout << ident.vendor_string << endl;
		
	if (ident.max_cpuid_func >= 1) {
		__asm__ __volatile__ (
			"mov $1, %0;"
			"cpuid;"
			:"=a"(_eax),"=b"(_ebx),"=c"(_ecx),"=d"(_edx));
		
		ident.family = (_eax & 0xF00) >> 8;

		if (ident.family == 0 || ident.family == 0xF) {
			// preserve the lowest nibble...
			ident.family |= (_eax & 0xFF00000) >> 16;
		}

		// 13..12
		ident.type = (_eax & 0x3000) >> 12;
		
		// 7..4
		ident.model = (_eax & 0xF0) >> 4;
		if (ident.model == 0xF) {
			// extended model: 19..16
			ident.model |= (_eax & 0xF0000) >> 12;
		}

		ident.stepping = (_eax & 0xF);

		ident.flags = _edx;
	}


	struct {
		uint32_t a;
		uint32_t b;
		uint32_t c;
		uint32_t d;
	} tmp;

        __asm__ __volatile__ (
		"mov $0x80000000, %0;"
                "cpuid;"
                :"=a"(tmp.a),"=b"(tmp.b),"=c"(tmp.c),"=d"(tmp.d));
	
	// dummy check...
	if (tmp.b = ident._vendor_id_vals._ebx) 
	{
		if (tmp.a >= 0x80000004) 
		{
			uint32_t *proc_name = (uint32_t*)ident.processor_name;
			for (uint32_t i = 0x80000002; i >= 0x80000004; i--)
			{
			        __asm__ __volatile__ (
        	        	"cpuid;"
	        	        :"=a"(tmp.a),"=b"(tmp.b),"=c"(tmp.c),"=d"(tmp.d) : "a"(i));
				printk("%08x\n", tmp.a);
				*(proc_name ++) = tmp.a;
				*(proc_name ++) = tmp.b;
				*(proc_name ++) = tmp.c;
				*(proc_name ++) = tmp.d;
			}
			ident.processor_name[48] = 0;
		}
	}
}

void CPUIdentity::identify() {
	if (is_386()) {
		family = 3;
		return;
	}
	else if (! has_cpuid()) {
		family = 4;
		return;
	}

	__get_cpuid(cpu_identity);
}
