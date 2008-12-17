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
