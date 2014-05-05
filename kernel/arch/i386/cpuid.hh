#ifndef __CPUID_INCLUDED__
#define __CPUID_INCLUDED__

#include <inttypes.h>

class CpuIdentity {
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
	CpuIdentity();
	void identify();
	const char *getVendor() const {
		return vendor_string;
	}
	const char *getProcessorName() const {
		return processor_name;
	}

	uint32_t getFlags() const { return flags; }
	int getType()       const { return type; }
	int getFamily()     const { return family; }
	int getModel()      const { return model; }
	int getStepping()   const { return stepping; }
	
	static const char *getFlagName(int);

	friend void __get_cpuid(CpuIdentity& ident);

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

extern CpuIdentity cpuIdentity;

#endif // __CPUID_INCLUDED__
