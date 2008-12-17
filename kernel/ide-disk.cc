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
		__pio_read_block(static_cast<uint16_t *>(dest));
		// block sizes can vary from 512?
		intptr_t tmp = reinterpret_cast<intptr_t>(dest);
		tmp += BLOCKSIZE;
		dest = reinterpret_cast<void *>(tmp);
	}
}

void IdeDisk::write(const void *src, size_t count)
{
	while (count --) {
		__pio_write_block(static_cast<const uint16_t *>(src));
		intptr_t tmp = reinterpret_cast<intptr_t>(src);
		tmp += BLOCKSIZE;
		src = reinterpret_cast<const void *>(tmp);
	}
}

void IdeDisk::command(struct ide_request_t request)
{
	__rw_command(request);
}

void IdeDisk::__pio_read_block(uint16_t *dest)
{
	for (int i = 0; i < BLOCKSIZE / 2; ++ i)
		dest[i] = inw(regbase + DATA);
	return;
}

void IdeDisk::__pio_write_block(const uint16_t *src)
{
	for (int i = 0; i < BLOCKSIZE / 2; ++ i)
		outw(regbase + DATA, src[i]);
	return;
}

void IdeDisk::__device_conf_identify()
{
	while (!inb(regbase + STATUS) & (1 << 6))
		; // spin while DRDY not set
	outb(regbase + FEATURE, 0xC2);
	outb(regbase + SELECT, drive << 4);
	outb(regbase + COMMAND, 0xB1);

	uint8_t status;
	while ((status = inb(regbase + STATUS)) & (1 << 7))
		; // spin while BSY set
	if (status & 1) // ERR set
		// INDICATE ERROR SOMEHOW
		return;
	// issue READ command to get identifying data
	return;
}

// FIXME: PACKET devices set ABORT and return PACKET signature
void IdeDisk::__identify_drive()
{
	while (!inb(regbase + STATUS) & (1 << 6))
		; // spin while DRDY not set
	outb(regbase + SELECT, drive << 4);
	outb(regbase + COMMAND, 0xEC);
	return;
}

void IdeDisk::__rw_command(struct ide_request_t request)
{
	unsigned long long block = request.block;
	size_t count = request.count;
	
	if (lba) {
		if (lba == LBA48) {
			outb(regbase + LBCOUNT, block >> 24);
			outb(regbase + MBCOUNT, block >> 32);
			outb(regbase + HBCOUNT, block >> 40);
			outb(regbase + LBCOUNT, block);
			outb(regbase + MBCOUNT, block >> 8);
			outb(regbase + HBCOUNT, block >> 16);
			// bits: [ obs, LBA, obs, DEV, Reserved(3:0) ]
			outb(regbase + SELECT, 0x40 | drive << 4);
			outb(regbase + NSECTOR, count >> 8);
		} else {
			outb(regbase + LBCOUNT, block);
			outb(regbase + MBCOUNT, block >> 8);
			outb(regbase + HBCOUNT, block >> 16);
			// bits: [ obs, LBA, obs, DEV, LBA(27:24) ]
			outb(regbase + SELECT, (0x40 | drive << 4) | (block >> 24 & 0x0F));
		}
	} else {
		uint32_t tmpblock, cylinder, head, sector, tmp;

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
