#ifndef __BLOCK_HH_INCLUDED__
#define __BLOCK_HH_INCLUDED__

#include "fs/filelike.hh"

class BlockDevice : public FileLike {
public:
	bool isSeekable() {
		return true;
	}
};

#endif
