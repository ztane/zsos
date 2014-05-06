// -*- C++ -*-

#ifndef IDE_DISK_INC
#define IDE_DISK_INC 1

#include <cstddef>
#include "ide-request.hh"
#include "ide-drive.hh"
#include "ide-ports.hh"
#include "kernel/ringbuffer.hh"

namespace ide {

class IdeInterface;

/* NOTE:
 * Create an IdeDrive interface that is used by IdeDisks
 */
class IdeDisk : public IdeDrive
{
	friend class IdeInterface;

private:
	void __rw_command(ide_request_t request);
	
public:
	IdeDisk(int ifnum, int drvnum);
	virtual ~IdeDisk();

	void read(void *dest, size_t count);
	void write(const void *src, size_t count);
	void command(struct ide_request_t request);
};

};

#endif
