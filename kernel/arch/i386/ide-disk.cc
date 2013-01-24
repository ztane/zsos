#include <cstdlib>
#include <iostream>
#include <stdint.h>
#include "kernel/ide-disk.hh"
#include "kernel/arch/current/port.h"

namespace ide {

IdeDisk::IdeDisk(int _ifnum, int _drvnum) : IdeDrive(_ifnum, _drvnum)
{
}

IdeDisk::~IdeDisk()
{
}

static const int BLOCKSIZE = 512;

void IdeDisk::read(void *dest, size_t count)
{
	while (count --) {
		__pio_read_block(dest);
		// block sizes can vary from 512?
		intptr_t tmp = reinterpret_cast<intptr_t>(dest);
		tmp += BLOCKSIZE;
		dest = reinterpret_cast<void *>(tmp);
	}
}

void IdeDisk::write(const void *src, size_t count)
{
	while (count --) {
		__pio_write_block(src);
		intptr_t tmp = reinterpret_cast<intptr_t>(src);
		tmp += BLOCKSIZE;
		src = reinterpret_cast<const void *>(tmp);
	}
	// Clear cache!
}

void IdeDisk::command(struct ide_request_t request)
{
	__rw_command(request);
}

void IdeDisk::__rw_command(struct ide_request_t request)
{
	unsigned long long block = request.block;
	size_t count = request.count;

	kout << "doing ide command..." << endl;
	if (lba) {
		kout << "using lba";
		if (lba == LBA48) {
			kout << "48..." << endl;
			outb(regbase + LBCOUNT, block >> 24);
			outb(regbase + MBCOUNT, block >> 32);
			outb(regbase + HBCOUNT, block >> 40);

			outb(regbase + NSECTOR, count >> 8);

			outb(regbase + LBCOUNT, block);
			outb(regbase + MBCOUNT, block >> 8);
			outb(regbase + HBCOUNT, block >> 16);

			// bits: [ obs, LBA, obs, DEV, Reserved(3:0) ]
			outb(regbase + SELECT, 0x40 | (drive << 4));
		} else {
			kout << "28..." << endl;
			outb(regbase + LBCOUNT, block);
			outb(regbase + MBCOUNT, block >> 8);
			outb(regbase + HBCOUNT, block >> 16);
			// bits: [ obs, LBA, obs, DEV, LBA(27:24) ]
			outb(regbase + SELECT, (0x40 | drive << 4) | (block >> 24 & 0x0F));
		}
	} else {
		uint32_t tmpblock, cylinder, head, sector, tmp;

		// Bail out if disk geometry is unknown
		if (! (geometry.heads && geometry.sectors)) {
			kout << "IdeDisk" << drive << ": Error: geometry not set!" << endl;
			return;
		}

		tmpblock = block;
		cylinder = tmpblock / (geometry.heads * geometry.sectors);
		tmp      = tmpblock % (geometry.heads * geometry.sectors);
		head     = tmp / geometry.sectors;
		sector   = tmp % geometry.sectors + 1;

		outb(regbase + LCYL, cylinder);
		outb(regbase + HCYL, cylinder >> 8);
		// bits: [ obs, 0 (LBA), obs, DEV, HEAD(3:0) ]
		outb(regbase + SELECT, drive << 4 | head & 0x0F);
		outb(regbase + SECTOR, sector);
	}
	outb(regbase + NSECTOR, count);
	
	if (request.type == ide_request_t::READ) {
		if (lba == LBA48)
			outb(regbase + COMMAND, 0x24); // READ SECTOR(S) EXT
		else
			outb(regbase + COMMAND, 0x20); // READ SECTOR(S)
	} else { // request.type == ide_request_t::WRITE
		if (lba == LBA48)
			outb(regbase + COMMAND, 0x34); // WRITE SECTOR(S) EXT
		else
			outb(regbase + COMMAND, 0x30); // WRITE SECTOR(S)
	}

	return;
}

};
