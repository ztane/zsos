#ifndef FILEDESCRIPTOR_HH_INCLUDED
#define FILEDESCRIPTOR_HH_INCLUDED

#include <stdint.h>
#include <stddef.h>

#include "kernel/types.hh"
#include "kernel/errnocode.hh"
#include "kernel/fs/filelike.hh"
#include "kernel/refcount.hh"

class FileDescriptor {
public:
	enum FileMode {
		READ     = 1,
		WRITE    = 2,
		APPEND   = 4,
		NONBLOCK = 8
	};

	enum FileModeExtra {
		SEEKABLE = 0x10000
	};

private:
	int          mode;
	FileLike    *target;
	FileOffset   offset;
	RefCount     counter;

public:
	FileDescriptor() : offset(0), counter() {
                target = NULL;
                mode   = 0;
        }

	static FileDescriptor *findFree();

	ErrnoCode doOpen(FileLike *file, int mode);

	ErrnoCode read(void *buf, size_t amount, size_t& read) {
		if (! mode & FileDescriptor::READ) {
			return EBADF;
		}

		ErrnoCode rc = this->target->read(buf, amount, offset, read);
		if (rc != NO_ERROR) {
			return rc;
		}

		this->offset += read;
		return rc;
	}

	ErrnoCode write(const void *buf, size_t amount, size_t& written) {
		if (! mode & FileDescriptor::WRITE) {
			return EBADF;
		}

		ErrnoCode rc = this->target->write(buf, amount, offset, written);
		if (rc != NO_ERROR) {
			return rc;
		}

		this->offset += written;
		return rc;
	}

	ErrnoCode seek(off_t offset, int whence) {
		if (! this->mode & SEEKABLE) {
			return EINVAL;
		}

		this->offset = offset;
	}

	void acquire() {
		counter ++;
	}

	bool release() {
		return --counter;
	}

	static ErrnoCode open(FileLike& file, int mode, FileDescriptor*& fd);

	FileLike *getInode() const {
		return this->target;
	}
};

#endif
