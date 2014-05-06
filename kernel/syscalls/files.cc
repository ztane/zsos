#include "kernel/syscall.hh"
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
	char           *name = (char*)r.arg0;
	struct stat    *st   = (struct stat*)r.arg1;
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
	int             fdn  = (int)r.arg0;
	struct stat    *st   = (struct stat*)r.arg1;

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
	char           *name = (char*)r.arg0;
	int             mode = r.arg1;
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
	int fdn = r.arg0;

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

SYSCALL(lseek)
{
	int fdn            = r.arg0;
	FileOffset           off((uint32_t)r.arg1);
	uint32_t whence    = r.arg2;
	FileOffset new_off(0);
	Task *current;
	FileDescriptor *fd;

	ErrnoCode rc = NOERROR;

	if (whence > 2) {
		rc = EINVAL;
		goto error_exit;
	}

	current = scheduler.getCurrentTask();
	fd = current->getFileDescriptor(fdn);

	// Not open?
	if (! fd) {
		rc = EBADF;
		goto error_exit;
	}
	else {
		rc = fd->lseek(off, whence, new_off);
		if (rc != NOERROR)
			goto error_exit;
	}

	SYSCALL_RETURN((uint32_t)new_off.to_scalar());

error_exit:
        SYSCALL_RETURN(-rc);
}
