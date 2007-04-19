#ifndef __EXE_BITS_HH__
#define __EXE_BITS_HH__

struct ZsosExeHeader {
	uint32_t magic1;
	uint32_t magic2;

	uint32_t hdrPhys;
	uint32_t hdrLength;

	uint32_t textPhys;
	uint32_t textVirt;
	uint32_t textLength;
	
	uint32_t dataPhys;
	uint32_t dataVirt;
	uint32_t dataLength;

	uint32_t bssPhys;
	uint32_t bssVirt;
	uint32_t bssLength;

	uint32_t entryPoint;
};

extern ZsosExeHeader _binary_example_zsx_start;
extern char _binary_example_zsx_end;

#endif
