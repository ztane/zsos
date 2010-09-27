#include <sys/stat.h>
#include <sys/times.h>
#include <stddef.h>
#include <stdint.h>
#include "syscall.h"

#include <errno.h>
#undef errno

extern int errno;

void _exit(int status) {
         __sys_exit(status);
         while (1) { };
}

int close(int file){
	return __sys_close(file);
}

int execve(char *name, char **argv, char **env){
        errno = ENOMEM;
        return -1;
}

int fork() {
        errno = EAGAIN;
        return -1;
}

int fstat(int file, struct stat *st) {
	return __sys_fstat(file, st);
}

int getpid() {
        return 1;
}

int isatty(int file){
        return 1;
}

int kill(int pid, int sig){
        errno=EINVAL;
        return(-1);
}

int link(const char *old, const char *new){
        errno=EMLINK;
        return -1;
}

int lseek(int file, int ptr, int dir) {
	return __sys_lseek(file, ptr, dir);
}

int open(const char *name, int flags, int mode){
	return __sys_open(name, flags, mode);
}

int read(int file, char *ptr, int len){
	return __sys_read(file, ptr, len);
}

int stat(const char *file, struct stat *st) {
        return __sys_stat(file, st);
}

clock_t times(struct tms *buf){
        return -1;
}

int unlink(char *name){
        errno=ENOENT;
        return -1;
}

int wait(int *status) {
        errno=ECHILD;
        return -1;
}

int write(int file, void *ptr, int len) {
	return __sys_write(file, ptr, len);
}

static void *__brkptr = 0x0;
int brk(void *newp) {
        void *rv;

        if (__brkptr == 0) {
                __brkptr = __sys_brk(0);
        }

        rv = __sys_brk(newp);
        if (newp != __brkptr && rv == __brkptr) {
                errno = ENOMEM;
                return -1;
        }

        // negative error?
        if (((uintptr_t)rv) > (uintptr_t)-128) {
                errno = ENOMEM;
                return -1;
        }

        __brkptr = rv;
        return 0;
}

void *sbrk(ptrdiff_t inc) {
        void *rv;
        void *res;

        if (__brkptr == 0) {
                __brkptr = __sys_brk(0);
        }
        rv = __brkptr;
        res = __sys_brk((char*)rv + inc);

        if (inc != 0 && res == __brkptr) {
                errno = ENOMEM;
                return (void*)-1;
        }

        // negative error?
        if (((uintptr_t)res) > (uintptr_t)-128) {
                errno = ENOMEM;
                return (void*)-1;
        }

        __brkptr = res;
        return rv;
}

