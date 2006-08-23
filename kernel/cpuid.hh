#ifndef __CPUID_INCLUDED__
#define __CPUID_INCLUDED__

#include <inttypes.h>

class CPUIdentity {
private:
	int family;
	int type;
	int model;
	int stepping;
	int vendor;

	uint32_t max_cpuid_func;
	uint32_t flags;
	union {
		struct {
			// definitely stupid ordering!!
			uint32_t _ebx;
			uint32_t _edx;
			uint32_t _ecx;
			char     terminator;
		} _vendor_id_vals;
		char vendor_string[13];
	};

	char processor_name[49];
public:
	CPUIdentity();
	void identify();
	const char *get_vendor() const {
		return vendor_string;
	}
	const char *get_processor_name() const {
		return processor_name;
	}

	uint32_t get_flags() const { return flags; }
	int get_type()     const { return type; }
	int get_family()   const { return family; }
	int get_model()    const { return model; }
	int get_stepping() const { return stepping; }
	
	static const char *get_flag_name(int);

	friend void __get_cpuid(CPUIdentity& ident);

	static const int FLAG_FPU = 1 << 0;
	static const int FLAG_PSE = 1 << 3;

//        "fpu",  "vme",  "de",   "pse",
//        "tsc",  "msr",  "pae",  "mce",
//        "cxchg8","apic","<unk>","sep",
//        "mtrr", "pge",  "mca",  "cmov",
//        "pat",  "pse36","psn",  "clfl",
//        "<unk>","dtes", "acpi", "mmx",
//        "fxsr", "sse",  "sse2", "ss",
//        "htt",  "tm1",  "ia-64","pbe",

};

extern CPUIdentity cpu_identity;

#endif // __CPUID_INCLUDED__
