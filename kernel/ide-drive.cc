#include <printk.h>
#include <iostream>
#include "kernel/ide-drive.hh"

namespace ide {

IdeDrive::IdeDrive(int _ifnum, int _drvnum) :
		drive(_drvnum),
		regbase(0x01F0 - _ifnum * 0x80),
		ctlbase(0x01F0 - _ifnum * 0x80 + 0x206)
{

}

IdeDrive::~IdeDrive()
{

}

void IdeDrive::__select()
{
	outb(regbase + SELECT, 0xA0 | drive << 4);
	// The interface requires ~400ns to select drive
	// cause delay with port reads
	inb(regbase + STATUS);
	inb(regbase + STATUS);
	inb(regbase + STATUS);
	inb(regbase + STATUS);
}

bool IdeDrive::__poll_pio_ready()
{
	int status;
	bool busy, drq, error;

	// NOTE: read alt-status register if using IRQ's
	// 0x80 BSY Busy
	// 0x01 ERR Error
	// 0x20 DF  Disk Failure
	// 0x08 DRQ Ready to accept PIO data / PIO data ready
	do {
		status = inb(regbase + STATUS);
		busy   = status & 0x80;
		error  = status & (0x01 | 0x20);
		drq    = status & 0x08;
	} while (busy && ! (drq || error));
	return drq; 
}
/**
 * FIXME: less agressive polling and timeout after 30s!
 */
bool IdeDrive::__poll_drive_ready()
{
	int status;
	do {
		status = inb(regbase + STATUS);
	} while (! (status & 0x40));
	return true;
}

void IdeDrive::__ata_soft_reset()
{
	// Send SRST (bit 2)
	outb(ctlbase + CONTROL, 0x04);
	// This will select drive0 of this interface
	// FIXME: ...we should set nIEN here if not using interrupts
	outb(ctlbase + CONTROL, 0);
	// wait till drive0 is ready
	inb(regbase + STATUS);
	inb(regbase + STATUS);
	inb(regbase + STATUS);
	inb(regbase + STATUS);
}

// FIXME: remove or move, datablock is always 256 words
static const size_t BLOCKSIZE = 256;

void IdeDrive::__pio_read_block(void *_dest)
{
	int16_t *dest = reinterpret_cast<int16_t *>(_dest);
	for (size_t i = 0; i < BLOCKSIZE; ++ i)
		dest[i] = inw(regbase + DATA);
	return;
}

void IdeDrive::__pio_write_block(const void *_src)
{
	const int16_t *src = reinterpret_cast<const int16_t *>(_src);
	for (size_t i = 0; i < BLOCKSIZE; ++ i)
		outw(regbase + DATA, src[i]);
	return;
}

/**
 * Please note that this lists all capabilities regardless if they've
 * been disabled with DEVICE CONFIGURATION SET
 */
void IdeDrive::__device_conf_identify()
{
	outb(regbase + FEATURE, 0xC2);
	outb(regbase + COMMAND, 0xB1);

	if (! __poll_pio_ready())
		return;

	__pio_read_block(identdata);
	return;
}

// FIXME: PACKET devices set ABORT and return PACKET signature
void IdeDrive::__identify_drive()
{
	outb(regbase + COMMAND, 0xEC);

	if (inb(regbase + STATUS) == 0) {
		exists = false;
		kout << "drive" << drive << " does not exist" << endl;
		return; // does not exist
	} else {
		exists = true;
	}

	if (! __poll_pio_ready())
		return; // some error

	__pio_read_block(identdata);
	return;
}

/**
 * Set this->geometry from identifying data
 */
void IdeDrive::__set_geometry()
{
	if (__has_lba48()) {
		kout << "drive" << drive << " has LBA48" << endl;
		lba = LBA48;
		geometry.max_lba = *((uint64_t *) &identdata[200]);
	} else if (__has_lba28()) {
		kout << "drive" << drive << " has LBA28" << endl;
		lba = LBA28;
		geometry.max_lba = *((uint32_t *) &identdata[120]);
	} else {
		kout << "drive" << drive << " has NO LBA" << endl;
		lba = NO_LBA;
		// Uhh do we really need these?
		geometry.max_lba = 0;
	}
}

void IdeDrive::read(void *dest, size_t count)
{

}

void IdeDrive::write(const void *src, size_t count)
{

}

void IdeDrive::command(struct ide_request_t request)
{

}

};
