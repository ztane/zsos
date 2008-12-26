	#include "zsosrdfs.hh"
#include <iostream>
#include <string.h>

class RdDirEntry {
public:
	char name[48];
	char type;
	char padding1;
	char padding2;
	char padding3;

	uint32_t size;
	uint32_t offset;

	uint32_t inode;
};

class RdFile : public FileLike {
private:
	ZsosRdFs *fs;
	uint32_t inode;
	uint32_t base;
	uint32_t length;
	uint32_t flags;
public:
	RdFile(ZsosRdFs *fs, uint32_t inode, uint32_t base, uint32_t length, bool isdir) {
		this->fs     = fs;
		this->inode  = inode;
		this->base   = base;
		this->length = length;
		this->flags  = isdir;
	}

	virtual bool isSeekable() {
		return true;
	}

	virtual bool isDir() {
		return flags & 1;
	}

	virtual ErrnoCode open(int mode, FileDescriptor*& desc) {
		return EINVAL;
	}

	virtual ErrnoCode write(const void *buf, size_t len, FileOffset off, size_t& written) {
		written = 0;
		return EINVAL;
	}

	virtual ErrnoCode read(void *buf, size_t to_read, FileOffset off, size_t& read) {
		if (off >= length) {
			read = 0;
			return NOERROR;
		}

		FileOffset bytes_left = FileOffset(length) - off;
		if (bytes_left <= to_read) {
			to_read = bytes_left.to_scalar();
		}

		return fs->container->read(buf, to_read, off + base, read);
	}
};

bool ZsosRdFsSuperblock::isValid() {
	if (strncmp(magic, "ZSOS RAMDISK TM", 16) != 0)
		return false;

	if (version.minor != 0 || version.major != 1)
		return false;

	if (page_size != 0x1000)
		return false;

	return true;
}

ErrnoCode ZsosRdFs::initialize(FileLike& container) {
	if (! container.isSeekable()) {
		return ErrnoCode(1);
	}

	super = new ZsosRdFsSuperblock();
	size_t bytes_read;

	ErrnoCode rc = container.read((void*)super, sizeof(*super), FileOffset(0), bytes_read);
	if (bytes_read != sizeof(*super)) {
		return ErrnoCode(2);
	}

	this->container = &container;
	if (! super->isValid()) {
		delete super;
		this->super = 0;
		return ErrnoCode(3);
	}

	return ErrnoCode(0);
}

ErrnoCode ZsosRdFs::findInode(uint32_t inode, FileLike*& container) {
	if (inode == 0) {
		container = new RdFile(this, 0, super->root_offset, super->root_size, true);
		return NOERROR;
	}

	// no error checking!
	RdDirEntry ent;
	size_t read = 0;

	// multiply by 64 to get offset
	ErrnoCode rc = this->container->read(&ent, 64, inode << 6, read);
	if (rc != NOERROR) {
		return ENOENT;
	}

	container = new RdFile(this, inode, ent.offset, ent.size, ent.type == 'd');
	return NOERROR;
}

ErrnoCode ZsosRdFs::openDir(FileLike& directory, FileDescriptor*& desc) {
	if (! directory.isDir()) {
		return ENOTDIR;
	}

        return FileDescriptor::open(directory, FileDescriptor::READ, desc);
}

ErrnoCode ZsosRdFs::readDir(FileDescriptor& desc, DirEntry& dentry) {
	RdDirEntry entry;
	size_t read;
	ErrnoCode rc;

	rc = desc.read(&entry, sizeof(entry), read);
	if (rc != NOERROR) {
		return rc;
	}

	if (read != sizeof(entry)) {
		dentry.setName("");
		dentry.setInode(0xFFFFFFFF);
	}
	else {
		dentry.setName(entry.name);
		dentry.setInode(entry.inode);
	}

	return NOERROR;
}

