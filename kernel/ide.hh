#ifndef IDE_HH
#define IDE_HH

#include <iostream>
#include <stdlib.h>
#include "interrupt.h"
#include "port.h"

struct hd_geometry {
	unsigned int cylinders;
	unsigned int heads;
	unsigned int sectors_track;
	unsigned int total_sectors;
	unsigned int block_size;
};

// HardDiskDevice
class HddDev
{
	public:
		HddDev();
		virtual ~HddDev();

		size_t read(void *, int, size_t);

	private:
		unsigned char   buf[512];
		unsigned short *bufw;

		unsigned int drive;
		unsigned int cylinder;
		unsigned int head;
		unsigned int sector;
		unsigned int sctr_count;

		struct hd_geometry geometry;
};

#endif
