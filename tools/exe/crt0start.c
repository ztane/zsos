#include <sys/syscall.h>
#include <unistd.h>

extern void (*__CTOR_LIST__)();
extern int main(int argc, char *argv[]);

static void call_ctors()
{
    void (**fptr)() = (&__CTOR_LIST__) + 1;
    int count = ((int *)(&__CTOR_LIST__))[0];
    while (count--)
    {
        (*fptr++)();
    }
}

int errno;

_syscall3(ssize_t, write, int, p1, const void*, p2
		, size_t, p3);

static _syscall1(void, exit, int, err);


void __CRT_startup() {
	char *argv[] = { "exe", 0 };
	call_ctors();
	write(1, "hello\n", 6);
	main(1, argv);
	exit(1);
}

