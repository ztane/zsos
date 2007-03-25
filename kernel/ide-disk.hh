// -*- C++ -*-

#ifndef IDE_DISK_INC
#define IDE_DISK_INC 1

namespace ide {

const size_t MAX_IDE_REQUESTS = 8; // make tunable

class IdeDrive
{
private:
	int drive;
	int regbase, ctlbase;

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
public:
	IdeDrive();
	~IdeDrive();
	int init(int ifnum, int drvnum);

	bool issueIdentifyDrive();
	bool issueRWCommand(size_t block, size_t count);
};

};

#endif
