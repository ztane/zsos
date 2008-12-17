#ifndef FILELIKE_HH_INCLUDED
#define FILELIKE_HH_INCLUDED

#include <stdint.h>
#include <stddef.h>
#include "kernel/errnocode.hh"

class FileOffset {
private:
	uint64_t pointer;
public:
	FileOffset(uint64_t pointer) {
                this->pointer = pointer;
        }

	FileOffset& operator+=(size_t increment) {
		this->pointer += increment;
		return *this;
	}

	FileOffset& operator-=(size_t decrement) {
		this->pointer -= decrement;
		return *this;
	}

	FileOffset& operator-=(const FileOffset& decrement) {
		this->pointer -= decrement.pointer;
		return *this;
	}

	FileOffset& operator+=(const FileOffset& increment) {
		this->pointer += increment.pointer;
		return *this;
	}
};

class FileDescriptor;

class FileLike {
private:
public:
	virtual bool isSeekable() = 0;

	virtual ErrnoCode open(int mode, FileDescriptor*& fd) = 0;
	virtual ErrnoCode read (void *buf, size_t amount, FileOffset offset) = 0;
	virtual ErrnoCode write(const void *buf, size_t amount, FileOffset offset) = 0;

	virtual ~FileLike() {
	}
};

#endif
