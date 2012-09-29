#ifndef __EXE_BITS_HH__
#define __EXE_BITS_HH__

#include <string.h>

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

extern uint8_t elfMagic[16];

class MemMap;

class ElfExeHeader {
public:
	static const int EI_NIDENT = 16;
protected:
	uint8_t  ident[EI_NIDENT];
	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint32_t entry;
	uint32_t phOff;
	uint32_t shOff;
	uint32_t flags;
	uint16_t ehSize;
	uint16_t phEntSize;
	uint16_t phNum;
	uint16_t shEntSize;
	uint16_t shNum;
	uint16_t shStrNdx;

public:
	bool isRunnable() {
		if (memcmp(ident, elfMagic, EI_NIDENT) != 0) {
			return false;
		}

		// executable
		if (type != 0x2) {
			return false;
		}

		// 386
		if (machine != 0x3) {
			return false;
		}

		// unknown version too!
		if (version != 1) {
			return false;
		}

		// no entry point!
		if (! entry) {
			return false;
		}

		if (! phOff) {
			return false;
		}
	}

	bool initializeMemMapAreas(MemMap *m);
        friend class UserTask;
};

extern ZsosExeHeader _binary_example_zsx_start;
extern char _binary_example_zsx_end;

#endif
