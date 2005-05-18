#include <stdlib.h>

// overload the operator "new"
void *operator new(unsigned int size)
{
    return kmalloc(size);
}

// overload the operator "new[]"
void *operator new[](unsigned int size)
{
    return kmalloc(size);
}

//overload the operator "delete"
void operator delete(void *p)
{
    kfree(p);
}

//overload the operator "delete[]"
void operator delete[](void * p)
{
    kfree(p);
}
