#include "filedescriptor.hh"

static const int num_fds = 1024;

// todo: implement differently?!
FileDescriptor fs_cache[num_fds];

FileDescriptor *FileDescriptor::findFree() {
	for (int i = 0; i < num_fds; i ++) {
		// FIXME: race.

		if (fs_cache[i].counter == 0) {
			fs_cache[i].counter ++;
			return fs_cache + i;
		}
	}

	return NULL;
}


ErrnoCode FileDescriptor::doOpen(FileLike *file, int mode) {
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

ErrnoCode FileDescriptor::open(FileLike& file, int mode, FileDescriptor*& fd) {
        FileDescriptor *free = FileDescriptor::findFree();
        if (! free) {
                return ENFILE;
        }

        fd = free;
        fd->doOpen(&file, mode);
        return NOERROR;
}
