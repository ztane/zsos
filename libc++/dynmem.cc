#include <cstdlib>
#include <kernel/mm/kmalloc.h>
#include <kernel/panic.hh>
#include <libutil/include/allocator>

static Allocator *default_allocator = 0;

// overload the operator "new"
void *operator new(size_t size)
{
	if (default_allocator == 0)
		kernelPanic("Default allocator not set");

	return default_allocator->allocate(size);
}

// overload the operator "new[]"
void *operator new[](size_t size)
{
	if (default_allocator == 0)
		kernelPanic("Default allocator not set");

	return default_allocator->allocate(size);
}

//overload the operator "delete"
void operator delete(void *p)
{
	return default_allocator->release(p);
}

//overload the operator "delete[]"
void operator delete[](void *p)
{
	return default_allocator->release(p);
}

void __set_default_allocator(Allocator *new_def)
{
	default_allocator = new_def;
}
