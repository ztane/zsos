#include <errno.h>
#include <stdlib.h>

char *getcwd(char *buf, size_t size) {
    if (size <= 1) {
        errno = EINVAL;
        if (size == 1)
            errno = ERANGE;
        return NULL;
    }
     
    buf[0] = '/';
    buf[1] = '\0';
    return buf;
}

int fsync(int fildes) {
    errno = EINVAL;
    return -1;
}

int symlink(const char *path1, const char *path2) {
    errno = EACCES;
    return -1;
}

#include <unistd.h>
uid_t getuid(void) {
    return 0;
}

gid_t getgid(void) {
    return 0;
}

pid_t getppid(void) {
    return 0;
}

uid_t geteuid(void) {
    return 0;
}

gid_t getegid(void) {
    return 0;
}

int chmod(const char *path, mode_t mode) {
    errno = EPERM;
    return -1;
}

char *ttyname(int fildes) {
    if (fildes <= 2) {
        return (const char*)"/dev/tty";
    }
    errno = ENOTTY;
    return NULL;
}

int access(const char *path, int amode) {
    errno = EACCES;
    return -1;
}

int pipe(int fildes[2]) {
    errno = ENFILE;
    return -1;
}

int fcntl(int fildes, int cmd, ...) {
    errno = EINVAL;
    return -1;
}

int dup(int fd) {
    errno = EMFILE;
    return -1;
}

int dup2(int fd1, int fd2) {
    errno = EBADF;
    return -1;
}

static mode_t umask_val = 0755;
mode_t umask(mode_t cmask) {
    mode_t rv;
    rv = umask_val;
    umask_val = cmask;
    return rv;
}

int fdatasync(int filedes) {
    return fsync(filedes);
}

int execv(const char *path, char *const argv[]) {
    errno = EINVAL;
    return -1;
}

int mkdir(const char *path, mode_t mode) {
    errno = EACCES;
    return -1;
}

int chdir(const char *path) {
    errno = EACCES;
    return -1;
}

int rmdir(const char *path) {
    errno = EACCES;
    return -1;
}

void *getpwnam() {
    return NULL;
}

void *getpwuid() {
    return NULL;
}
