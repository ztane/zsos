extern void (*__CTOR_LIST__)();
extern int main(int argc, char *argv[]);

void __CRT_startup() {
	char *argv[] = { "exe", 0 };
	main(1, argv);
	// exit 1?
}

static void call_ctors()
{
    void (**fptr)() = (&__CTOR_LIST__) + 1;
    int count = ((int *)(&__CTOR_LIST__))[0];
    while (count--)
    {
        (*fptr++)();
    }
}

