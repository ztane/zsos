#ifndef __DIRENTRY_CC_INCLUDED__
#define __DIRENTRY_CC_INCLUDED__

#define PATH_MAX                 256

#include <inttypes.h>
#include <string.h>

class DirEntry {
private:
	uint64_t inode;
	char     dname[PATH_MAX + 1];
public:
	DirEntry(char *name, uint32_t inode) { setName(name); this->inode = inode; };
	DirEntry() { dname[0] = 0; inode = 0xFFFFFFFF; };

	char *getName()          { return dname; }
	uint64_t getInode()         { return inode; }
	void setName(char *name) { strncpy(dname, name, sizeof(dname)); }
	void setInode(uint64_t inode) { this->inode = inode; }
	bool empty() 		 { return ! dname[0]; }
};

#endif
