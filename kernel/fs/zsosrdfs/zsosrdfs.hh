#ifndef __ZSOSRDFS_HH_INCLUDED__
#define __ZSOSRDFS_HH_INCLUDED__

#include <inttypes.h>
#include "kernel/fs/filelike.hh"
#include "kernel/fs/filedescriptor.hh"
#include "kernel/fs/direntry.hh"

class RdFile;

class ZsosRdFsSuperblock {
private:
	char magic[16];
	struct {
		uint16_t minor;
		uint16_t major;
	} version;

	uint32_t page_size;
	char label[64];
	char uuid[16];

	uint32_t root_offset;
	uint32_t root_size;

public:
	bool isValid();

	friend class ZsosRdFs;
};

class ZsosRdFs {
private:
	ZsosRdFsSuperblock *super;
	FileLike *container;
	FileOffset maxOffset;
public:

	ZsosRdFs() : super(0), container(0), maxOffset(0) {
	};

	ErrnoCode initialize(FileLike& container);
	ErrnoCode uninitialize();

	ErrnoCode findInode(uint32_t inode, FileLike*& file);
	ErrnoCode findPath(char *path, uint32_t& inode);
//	ErrnoCode 

	ErrnoCode openDir(FileLike& directory, FileDescriptor*& desc);
	ErrnoCode readDir(FileDescriptor& desc, DirEntry& entry);

	friend class RdFile;
};

#endif
