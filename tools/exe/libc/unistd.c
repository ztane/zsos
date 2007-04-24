#include <errno.h>
#include <unistd.h>

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

