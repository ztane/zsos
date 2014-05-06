#ifndef _IDE_DRIVE_
#define _IDE_DRIVE_ 1

#include <cstddef>
#include <cstring>
#include "ide-ports.hh"
#include "ide-request.hh"

// TODO: rename everything "IDE" as "ATA" since the former is electrical spec and
// latter software spec!

namespace ide {

class IdeInterface;

class IdeDrive
{
	friend class IdeInterface;

protected:
	const int drive, regbase, ctlbase;
	
	bool exists;

	enum {
		NO_LBA,
		LBA28,
		LBA48
	} lba;

	struct {
		int cylinders;
		int heads;
		int sectors;
		unsigned long long max_lba;
	} geometry;
	
	char identdata[512];
	
	void __select();
	bool __poll_pio_ready();
	bool __poll_drive_ready();
	void __ata_soft_reset();
	void __pio_read_block(void *dest);
	void __pio_write_block(const void *src);
	void __device_conf_identify();
	void __identify_drive();

	bool __has_lba28() {
		// Words 60 & 61 as DWORD contain max lba28 addressable sectors, if supported, else 0
		uint32_t maxlba28;
                memcpy(&maxlba28, &identdata[120], sizeof(maxlba28));
		return maxlba28 != 0;
	}

	bool __has_lba48() {
		// Word 83 of identify data has bit 10 set if drive supports LBA48
		return identdata[167] & 0x04;
	}

	void __set_geometry();

public:
	IdeDrive(int ifnum, int drvnum);
	virtual ~IdeDrive();

	virtual void read(void *dest, size_t count);
	virtual void write(const void *src, size_t count);
	virtual void command(struct ide_request_t request);

	bool get_exists() {
		return exists;
	}
};

};

#endif
