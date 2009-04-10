#ifndef FILELIKE_HH_INCLUDED
#define FILELIKE_HH_INCLUDED

#include <stdint.h>
#include <stddef.h>
#include "kernel/errnocode.hh"

class FileOffset {
private:
	int64_t pointer;
public:
	FileOffset(uint64_t pointer) {
                this->pointer = pointer;
        }

	FileOffset& operator-=(const FileOffset& decrement) {
		this->pointer -= decrement.pointer;
		return *this;
	}

	FileOffset& operator+=(const FileOffset& increment) {
		this->pointer += increment.pointer;
		return *this;
	}

	const FileOffset operator-(const FileOffset& decrement) const {
		return FileOffset(this->pointer - decrement.pointer);
	}

	const FileOffset operator+(const FileOffset& increment) const {
		return FileOffset(this->pointer + increment.pointer);
	}

	bool operator<(const FileOffset& other) const {
		return this->pointer < other.pointer;
	}

	bool operator==(const FileOffset& other) const {
		return this->pointer == other.pointer;
	}

	bool operator>(const FileOffset& other) const {
		return this->pointer > other.pointer;
	}

	bool operator<=(const FileOffset& other) const {
		return this->pointer <= other.pointer;
	}

	bool operator!=(const FileOffset& other) const {
		return this->pointer != other.pointer;
	}

	bool operator>=(const FileOffset& other) const {
		return this->pointer >= other.pointer;
	}

	int64_t to_scalar() const {
		return this->pointer;
	}
};

class FileDescriptor;

class FileLike {
private:
public:
	virtual bool isSeekable() = 0;
	virtual bool isDir() { return false; }
	virtual FileOffset getSize() { return FileOffset(0); };
	virtual ErrnoCode open(int mode, FileDescriptor*& fd) = 0;
	virtual ErrnoCode read (void *buf, size_t amount, FileOffset offset, size_t& read) = 0;
	virtual ErrnoCode write(const void *buf, size_t amount, FileOffset offset, size_t& written) = 0;

	virtual int getInode() {
		return 0;
	}

	virtual ~FileLike() {
	}
};

#endif
