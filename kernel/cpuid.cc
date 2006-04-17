#include <cpuid>
#include <printk.h>
#include <iostream>

CPUIdentity cpu_identity;

CPUIdentity::CPUIdentity() {
	family = model = type = stepping = 0;
	_vendor_id_vals._ebx = 0;
	
	max_cpuid_func = 0;	
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

	if (ident.max_cpuid_func >= 1) {
		__asm__ __volatile__ (
			"mov $1, %0;"
			"cpuid;"
			: "=a"(_eax), "=b"(_ebx), "=c"(_ecx), "=d"(_edx));
		
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

	kout << "Has cpuid..." << endl;	
	__get_cpuid(cpu_identity);
}
