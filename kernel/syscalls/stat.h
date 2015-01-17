#ifndef __STAT_H__INCLUDED__
#define __STAT_H__INCLUDED__

#include <inttypes.h>

#define S_IFMT     0170000  // bit mask for the file type bit fields
#define S_IFSOCK   0140000  // socket
#define S_IFLNK    0120000  // symbolic link
#define S_IFREG    0100000  // regular file
#define S_IFBLK    0060000  // block device
#define S_IFDIR    0040000  // directory
#define S_IFCHR    0020000  // character device
#define S_IFIFO    0010000  //  FIFO
#define S_ISUID    0004000  // set UID bit
#define S_ISGID    0002000  // set-group-ID bit (see below)
#define S_ISVTX    0001000  // sticky bit (see below)

struct stat
{
  uint16_t st_dev;
  uint16_t st_ino;
  uint32_t st_mode;
  uint16_t st_nlink;
  uint16_t st_uid;
  uint16_t st_gid;
  uint16_t st_rdev;
  uint32_t st_size;

  uint32_t st_atime;
   int32_t st_spare1;
  uint32_t st_mtime;
   int32_t st_spare2;
  uint32_t st_ctime;
   int32_t st_spare3;
   int32_t st_blksize;
   int32_t st_blocks;
   int32_t st_spare4[2];
};

// from linux headers!

struct stat64 {
    unsigned long long st_dev;
    unsigned char      __pad0[4];
    unsigned long      __st_ino;
    unsigned int       st_mode;
    unsigned int       st_nlink;
    unsigned long      st_uid;
    unsigned long      st_gid;
    unsigned long long st_rdev;
    unsigned char      __pad3[4];
    long long          st_size;
    unsigned long      st_blksize;
    unsigned long long st_blocks;   /* Number 512-byte blocks allocated. */
    unsigned long      st_atime;
    unsigned long      st_atime_nsec;
    unsigned long      st_mtime;
    unsigned long      st_mtime_nsec;
    unsigned long      st_ctime;
    unsigned long      st_ctime_nsec;
    unsigned long long st_ino;
};

#endif
