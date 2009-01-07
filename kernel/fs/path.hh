#ifndef __DIRECTORY_HH__
#define __DIRECTORY_HH__

#include "kernel/fs/direntry.hh"
#include "kernel/errnocode.hh"
#include "kernel/fs/filelike.hh"

ErrnoCode resolvePath(char pathname[PATH_MAX + 1], FileLike*& resolved);

#endif
