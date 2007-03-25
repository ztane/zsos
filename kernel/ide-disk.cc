#include <cstdlib>
#include <iostream>
#include "kernel/ide-disk.hh"
#include "kernel/ide-ports.hh"
#include "kernel/arch/current/port.h"

namespace ide {

IdeDrive::IdeDrive() {}
IdeDrive::~IdeDrive() {}

int IdeDrive::init(int ifnum, int drvnum)
{
	drive = drvnum;
	regbase = 0x1f0 - ifnum * 0x80;
	ctlbase = regbase + 0x206;

	lba = NO_LBA;
	geometry.cylinders = 0;
	geometry.heads = 0;
	geometry.sectors = 0;
	geometry.max_lba = 0;

	return 0;
}

bool IdeDrive::issueIdentifyDrive()
{
	outb(regbase + COMMAND_OFFSET, 0xec); // 0xec - identify drive
	return true;
}

bool IdeDrive::issueRWCommand(size_t block, size_t count)
{
	if (lba) {
		if (lba == LBA48) {

		} else {

		}
	} else {

	}

	return true;
}

};
