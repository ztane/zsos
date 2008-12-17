#ifndef _IDE_DRIVE_
#define _IDE_DRIVE_ 1

#include <cstddef>
#include "kernel/ide-request.hh"

namespace ide {

class IdeDrive
{
protected:
	const int drive, regbase, ctlbase;
public:
	IdeDrive(int ifnum, int drvnum);
	virtual ~IdeDrive();

	virtual void read(void *dest, size_t count);
	virtual void write(const void *src, size_t count);
	virtual void command(struct ide_request_t request);
};

};

#endif
