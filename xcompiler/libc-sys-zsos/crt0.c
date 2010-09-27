extern int main(int argc, char *argv[]);
extern void (*__CTOR_LIST__)();

static void call_ctors();

extern char **environ;
char *__env[1] = { 0 };

void _start() {
	char *argv[] = { "exe", 0 };
        environ = __env;

	call_ctors();
	exit(main(1, argv));
}

#include "syscall.h"

static void call_ctors()
{
        void (**fptr)() = (&__CTOR_LIST__) + 1;
        void *p = (void *)&__CTOR_LIST__;
        int count = *(int*)p;
        while (count--)
        {
                (*fptr++)();
        }
}

