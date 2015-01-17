#include "kernel/syscall.hh"
#include "kernel/printk.h"
#include "kernel/errnocode.hh"
#include "kernel/fs/filedescriptor.hh"
#include "kernel/fs/path.hh"
#include "kernel/mm/filepageloader.hh"

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

void fillStat64Struct(struct stat64 *st, FileLike* from) {
	memset(st, 0, sizeof(struct stat64));

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

SYSCALL(access)
{
	char *name = (char*)r.arg0;
	struct stat st;
	ErrnoCode       rc = NOERROR;

        FileLike *file;
	rc = resolvePath(name, file);
        if (rc != NOERROR) {
            goto exit;
        }

        if (r.arg1 == 0 || r.arg1 == 4 || r.arg1 == 1) { // F_OK, R_OK, X_OK
            goto exit;
        }

        if (r.arg1 == 2) { // W_OK
            rc = EACCES;
        }
        else {
            rc = EINVAL;
        }

exit:
	SYSCALL_RETURN(-rc);
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
	if (rc != NOERROR) {
            goto exit;
        }

	fillStatStruct(st, file);

exit:
	SYSCALL_RETURN(-rc);
}

SYSCALL(fstat)
{
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

SYSCALL(fstat64)
{
	// todo: check pointer
	int             fdn  = (int)r.arg0;
	struct stat64  *st   = (struct stat64*)r.arg1;

	ErrnoCode       rc = NOERROR;
	FileLike       *file;

	Task *current = scheduler.getCurrentTask();
	FileDescriptor *fd = current->getFileDescriptor(fdn);

	// Not open?
	if (! fd) {
		rc = EBADF;
	}
	else {
		fillStat64Struct(st, fd->getInode());
	}

	SYSCALL_RETURN(-rc);
}

#ifndef O_CLOEXEC
# define O_CLOEXEC 02000000
#endif

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

	// TODO: support this
	mode &= ~O_CLOEXEC;

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

#define MAP_DENYWRITE   0x0800
#define MAP_EXECUTABLE  0x1000
#define MAP_SHARED      0x01
#define MAP_PRIVATE     0x02
#define MAP_FIXED       0x10
#define MAP_ANONYMOUS   0x20

#define PROT_READ       0x1             /* page can be read */
#define PROT_WRITE      0x2             /* page can be written */
#define PROT_EXEC       0x4

SYSCALL(mmap2) {
	uint32_t addr = r.arg0;
	uint32_t length = r.arg1;
	uint32_t prot = r.arg2;
	uint32_t flags = r.arg3;
	int32_t  fdn = r.arg4;
	uint32_t offset = r.arg5;
	uint32_t rc = 0;
	bool fixedAddr = false;
	FileLike *file = NULL;
	Task *current;
	MemMapArea *area = NULL;

	flags &= ~ MAP_DENYWRITE; // unused

	// need to have map private and nonzero length
	if ((! (flags & MAP_PRIVATE)) || ! length) {
		rc = EINVAL;
		goto error_exit;
	}

	if (flags & MAP_FIXED) {
		// not page granular
		if (addr & Paging::PAGE_OFFSET_MASK) {
			rc = EINVAL;
			goto error_exit;
		}

		fixedAddr = true;
	}

	if (flags & MAP_ANONYMOUS) {
		fdn = -1;
	}

	flags &= ~ (MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS | MAP_EXECUTABLE);
	if (flags) {
		rc = EINVAL;
		goto error_exit;
	}

	current = scheduler.getCurrentTask();
	if (fdn != -1) {
		FileDescriptor *fd = NULL;
		fd = current->getFileDescriptor(fdn);
		if (! fd) {
			rc = EBADF;
			goto error_exit;
		}

		file = fd->getInode();
		if (file->isDir()) {
			rc = EACCES;
			goto error_exit;
		}
	}

	// goto frame
	{
		if (fixedAddr) {
			area = new MemMapArea(VirtAddr((void*)addr), (VirtAddr((void*)addr) + length - 1).getNextPage());
		}
		else {
			area = new MemMapArea(VirtAddr(0), VirtAddr((char *)length - 1).getNextPage());
		}

		if (file) {
			area->setOffset(offset);
			area->setFile(file);

			int64_t size = file->getSize().to_scalar();
			int64_t offsetScaled = (int64_t)offset << Paging::PAGE_SHIFT;

			VirtAddr zeroLimit = area->getEnd();
			if (size <= offsetScaled) {
				area->setZeroStart(area->getBase());
			}
			else {
				size -= offsetScaled;
				if (size < length) {
					area->setZeroStart(area->getBase() + size);
				}
				else {
					area->setZeroStart(area->getBase() + length);
				}
			}
		}
		else {
			area->setZeroStart(area->getBase());
		}

		MemMapArea::Prot access = MemMapArea::RX;
		if (prot & PROT_WRITE) {
			access = MemMapArea::RWX;
		}
		area->setProt(access);
		area->setLoader(&filePageLoader);


		MemMap *memmap = current->getMemMap();

		if (fixedAddr) {
			rc = memmap->addAreaAt(area);
		}
		else {
			// locates the map in mem
			rc = memmap->addArea(area);
		}

		if (rc == 0) {
			SYSCALL_RETURN(area->getBase().toInteger());
		}
	}
error_exit:
	if (area) {
		delete area;
	}

	SYSCALL_RETURN(-rc);
}
