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
