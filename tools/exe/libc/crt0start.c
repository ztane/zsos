#include <sys/syscall.h>

extern void (*__CTOR_LIST__)();
extern int main(int argc, char *argv[]);

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

void __CRT_startup() {
	char *argv[] = { "exe", 0 };
	call_ctors();
	int rv = main(1, argv);
	exit(rv);
}

