#include <cstdlib>

// overload the operator "new"
void *operator new(size_t size)
{
    return kmalloc(size);
}

// overload the operator "new[]"
void *operator new[](size_t size)
{
    return kmalloc(size);
}

//overload the operator "delete"
void operator delete(void *p)
{
    kfree(p);
}

//overload the operator "delete[]"
void operator delete[](void *p)
{
    kfree(p);
}
