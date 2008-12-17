// -*- C++ -*-

#ifndef IDE_DISK_INC
#define IDE_DISK_INC 1

#include <cstddef>
#include "kernel/ide-request.hh"
#include "kernel/ide-drive.hh"
#include "kernel/ide-ports.hh"
#include "kernel/ringbuffer.hh"

namespace ide {

/* NOTE:
 * Create an IdeDrive interface that is used by IdeDisks
 */
class IdeDisk : public IdeDrive
{
private:
	enum {
		NO_LBA,
		LBA28,
		LBA48
	} lba;

	struct {
		int cylinders;
		int heads;
		int sectors;
		int max_lba;
	} geometry;

	void __pio_read_block(uint16_t *dest);
	void __pio_write_block(const uint16_t *src);
	void __device_conf_identify();
	void __identify_drive();
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
