#ifndef IDE_HH
#define IDE_HH

#include <iostream>
#include "interrupt.h"
#include "port.h"

// HardDiskDevice
class HddDev
{
	public:
		HddDev();
		virtual ~HddDev();

		void read();

	private:
		unsigned int drive;
		unsigned int cylinder;
		unsigned int head;
		unsigned int sector;
		unsigned int sctr_count;

		unsigned char   buf[512];
		unsigned short *bufw;
};

#endif
