#include <string.h>
#include <iostream>

#include "path.hh"
#include "kernel/fs/zsosrdfs/zsosrdfs.hh"
#include "kernel/errnocode.hh"

#define PATH_COMP_MAX 48

extern ZsosRdFs       root_filesystem;

ErrnoCode resolvePath(char pathname[PATH_MAX + 1], FileLike*& resolved) {
	if (strlen(pathname) > PATH_MAX) {
		return ENAMETOOLONG;
	}

	char working_buf[PATH_MAX + 1];
	char *cur_ptr = working_buf;
	char *component;

	strncpy(working_buf, pathname, PATH_MAX + 1);

	// start with root inode
	uint32_t inode = 0;

	component = strsplit(&cur_ptr, '/');
	ErrnoCode rc = NOERROR;

	DirEntry entry;
	int loops = 0;
	while (component) {
		if (*component) {
			FileLike *dir;
			rc = root_filesystem.findInode(inode, dir);
			if (rc != NOERROR)
				break;

			if (! dir->isDir()) {
				rc = ENOENT;

				// todo: free inode / filelike
				break;
			}

			FileDescriptor *dirfd;
			rc = root_filesystem.openDir(*dir, dirfd);
			if (! dir->isDir()) {
				// free inode
				break;
			}

			rc = root_filesystem.readDir(*dirfd, entry);

			bool found = false;
			while (rc == NOERROR && ! entry.empty()) {
				if (strncmp(entry.getName(), component, PATH_COMP_MAX) == 0) {
					found = true;
					inode = entry.getInode();
					break;
				}

				rc = root_filesystem.readDir(*dirfd, entry);
			}

			loops ++;
			dirfd->release();

			if (rc != NOERROR)
				break;

			if (! found) {
				rc = ENOENT;
				break;
			}
		}

		component = strsplit(&cur_ptr, '/');
	}

	if (rc == NOERROR) {
		return root_filesystem.findInode(inode, resolved);
	}

	return rc;
}

