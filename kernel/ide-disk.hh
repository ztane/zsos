// -*- C++ -*-

#ifndef IDE_DISK_INC
#define IDE_DISK_INC 1

namespace IDE {


// Perhpas IDEDrive should be generic interface for disk/cdrom/tape/whatever
class IDEDrive
{
public:
	IDEDrive();
	~IDEDrive();

	int init();
};


};
#endif
