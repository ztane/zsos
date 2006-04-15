#include <allocator>

void *operator new(size_t size, Allocator& alloc)
{
	return alloc.allocate(size);
}

void *operator new[] (size_t size, Allocator& alloc) 
{
	return alloc.allocate(size);
}

