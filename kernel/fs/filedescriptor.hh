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

	ErrnoCode open(FileLike *file, int mode) {
		this->target = file;

		// target is acquired, and opened with mode.
		this->mode = mode;
		this->counter.set1();

		if (target->isSeekable()) {
			mode |= SEEKABLE;
		}

		// fixme
		this->offset = FileOffset(0);
		return NOERROR;
	}

	ErrnoCode read(void *buf, size_t amount) {
		if (! mode & FileDescriptor::READ) {
			return EBADF;
		}

		ErrnoCode rc = this->target->read(buf, amount, offset);
		if (rc != NO_ERROR) {
			return rc;
		}

		this->offset += amount;
		return rc;
	}

	ErrnoCode write(const void *buf, size_t amount) {
		if (! mode & FileDescriptor::WRITE) {
			return EBADF;
		}

		ErrnoCode rc = this->target->write(buf, amount, offset);
		if (rc != NO_ERROR) {
			return rc;
		}

		this->offset += amount;
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

	bool release();
};

#endif
