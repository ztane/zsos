#include "kernel/syscall.hh"
#include "kernel/initial_vga.h"
#include "kernel/printk.h"
#include "kernel/errnocode.hh"
#include "kernel/fs/filedescriptor.hh"
#include "kernel/fs/path.hh"

#include "stat.h"

#include <iostream>
#include <scheduler.hh>

extern Scheduler scheduler;

void fillStatStruct(struct stat *st, FileLike* from) {
	memset(st, 0, sizeof(struct stat));

        st->st_dev   = 1;
	st->st_ino   = from->getInode();

	// char devices? blk devices?
	st->st_mode  = 0555 | (from->isDir() ? S_IFDIR : S_IFREG);
        st->st_nlink = 1;

	// uid, gid, rdev = 0

	st->st_size  = from->getSize().to_scalar();

//      a,m,ctime = 0

	st->st_blksize = 512;
	st->st_blocks  = (st->st_size + 511) / 512;
}

// SERVES ALSO AS LSTAT FOR NOW!
SYSCALL(stat)
{
	// todo: check pointerS
	char           *name = (char*)r.ebx;
	struct stat    *st   = (struct stat*)r.ecx;
	ErrnoCode       rc = NOERROR;
	FileLike       *file;

	rc = resolvePath(name, file);
	if (rc != NOERROR)
		goto exit;

	fillStatStruct(st, file);
exit:
	SYSCALL_RETURN(-rc);
}

SYSCALL(fstat)
{
//	SYSCALL_RETURN(-EACCES);

	// todo: check pointer
	int             fdn  = (int)r.ebx;
	struct stat    *st   = (struct stat*)r.ecx;

	ErrnoCode       rc = NOERROR;
	FileLike       *file;

	Task *current = scheduler.getCurrentTask();
	FileDescriptor *fd = current->getFileDescriptor(fdn);

	// Not open?
	if (! fd) {
		rc = EBADF;
	}
	else {
		fillStatStruct(st, fd->getInode());
	}

	SYSCALL_RETURN(-rc);
}

SYSCALL(open)
{
	// todo: check pointer
	char           *name = (char*)r.ebx;
	int             mode = r.ecx;
        Task           *current = scheduler.getCurrentTask();
	ErrnoCode       rc = NOERROR;
	FileLike       *file;
	FileDescriptor *fd;
	int             number;

	rc = resolvePath(name, file);
	if (rc != NOERROR)
		goto exit;

	// no writing, nothing!
	if (mode) {
		rc = EACCES;
		goto exit;
	}

	rc = FileDescriptor::open(*file, FileDescriptor::READ, fd);
	if (rc != NOERROR)
		goto exit;

	number = current->findFreeFdSlot(fd);
	if (number == -1) {
		rc = EMFILE;
		fd->release();
		goto exit;
	}

	SYSCALL_RETURN(number);

exit:
	SYSCALL_RETURN(-rc);
}

SYSCALL(close)
{
	int fdn = r.ebx;

	Task *current = scheduler.getCurrentTask();
	FileDescriptor *fd = current->getFileDescriptor(fdn);

	int rc = NOERROR;

	// Not open?
	if (! fd) {
		rc = EBADF;
	}
	else {
		// todo: fix race! what happens if closed twice!
		// and released!!! urgh
		current->fileDescriptors[fdn] = 0;
		fd->release();
	}
        SYSCALL_RETURN(-rc);
}
