#include <stdlib.h>
#include <string.h>
#include "kmalloc.h"
#include <panic>

/**
 * TODO: Heap in general should be PAGE aligned (4k in x86)
 */
#define FREEMEM_LISTS 29
#define FREEMEM_MIN   (sizeof(freeblock_t) - sizeof(usedblock_t))
#define HEAP_PADDING  (sizeof(intptr_t))

typedef union _memory_block *memblock_ptr;

typedef struct _used_block {
	memblock_ptr prev;
	memblock_ptr next;
} usedblock_t;
	
typedef struct _free_block {
	memblock_ptr next;
	memblock_ptr prev;
	memblock_ptr next_free;
	memblock_ptr prev_free;
} freeblock_t;

typedef union _memory_block {
	memblock_ptr first;
	usedblock_t  used;
	freeblock_t  free;
} memory_block_t;

static int kmalloc_initialized = 0;

static freeblock_t freemem_bp[FREEMEM_LISTS]; /* list of free blocks */

/**
 * for <= 0x80000000 returns 28 and
 * for        >= 0x8 returns 0
 */
static inline int _get_first_bit_set(size_t value)
{
	unsigned int test = 0x80000000;
	int bitpos = FREEMEM_LISTS - 1;

        for ( ; test && bitpos; bitpos --, test >>= 1)
                if (value & test)
                        break;
	return bitpos;
}

/**
 * Used by _kmalloc_fast.
 * scan memory for best fitting block
 * return correct block or NULL on failure
 */
static memblock_ptr _kmalloc_best_fit(size_t size, int size_index)
{
	size_t temp, overhead = 0;	/* init to known value */
	memblock_ptr bp, rval = NULL;
	/* rval is the return value. init as NULL, on failure return NULL */

	for (bp = freemem_bp[size_index].next_free;
		(void *) bp != (void *) &freemem_bp[size_index];
		bp = bp->free.next_free)
	{
		temp  = (void *) bp->free.next - (void *) bp - sizeof(usedblock_t);
		if ((temp - size) < 0)
			continue;
		if ((temp - size) == 0)
		{
			rval = bp;
			break;
		}
		/* if overhead is 0 we wouldn't be here! */
		if ((temp - size) < overhead || overhead == 0)
		{
			rval = bp;
			overhead = temp - size;
		}
	}
	
	return rval;
}

/**
 * Used by kmalloc and others.
 * First try to find a block from the larger block lists.
 * Return NULL on failure, void * to memory area on success.
 */
static void *_kmalloc_fast(size_t size, int size_index)
{
	int fsize_index = size_index; /* store original size_index for later use */
	size_t fsize;
	memblock_ptr bp = NULL, t_block; /* t_block = the new freeblock_t element */

	/* just to be shure (the for loop might go nuts) */
	if (size_index < 0)
		return NULL;
	
	/* This will in optimal situation be a O(1) algorithm
	 * scan through the freemem lists. the first block in a list
	 * should be good if available */
	for (++size_index; size_index < FREEMEM_LISTS; size_index++) {
		bp = freemem_bp[size_index].next_free;
		if ((void *) bp != (void *) &freemem_bp[size_index])
			break;
	}
	/* If size_index is FREEMEM_LISTS, then there were no free blocks
	 * in the larger lists. Try to find space from the blocks own
	 * free list with best fit. */
	if (size_index == FREEMEM_LISTS)
	{
		bp = _kmalloc_best_fit(size, fsize_index);
	}
	if (!bp)
		return NULL;
	
	fsize   = (void *) bp->free.next - (void *) bp;
	fsize  -= sizeof(usedblock_t);
	fsize  -= size - sizeof(usedblock_t);
	/* sizeof freeblock_t = 2 * sizeof usedblock_t */
	t_block = fsize < FREEMEM_MIN ? NULL :
		(memblock_ptr) ((void *) bp + sizeof(usedblock_t) + size);
	/* keep the 8 byte blocks possible, since they might be needed! */
	
	/* unlink bp from free block list */
	bp->free.next_free->free.prev_free = bp->free.prev_free;
	bp->free.prev_free->free.next_free = bp->free.next_free;

	if (t_block)
	{
		t_block->free.prev = bp;
		t_block->free.next = bp->free.next;
		t_block->free.next->used.prev = t_block;
		bp->used.prev = bp->free.prev;
		bp->used.next = t_block;

		fsize_index = _get_first_bit_set(fsize);

		/* link the new free block to correct list */
		t_block->free.prev_free = (memblock_ptr) &freemem_bp[fsize_index];
		t_block->free.next_free = freemem_bp[fsize_index].next_free;
		t_block->free.prev_free->free.next_free = t_block;		
		t_block->free.next_free->free.prev_free = t_block;
	} else
	{
		/* in this case the 'next' block is already
		 * pointing at 'bp' */
		t_block = bp->free.next;
		bp->used.prev = bp->free.prev;
		bp->used.next = t_block;
	}

	return (void *) ((void *) bp + sizeof(usedblock_t));
}

/**
 * Allocate a block of memory of given size. Return a pointer
 * to start of usable block.
 * 
 * @param size_t size
 * @return void pointer to start of block
 */
void *kmalloc(size_t size)
{
	if (!size)
		return NULL;

	if (! kmalloc_initialized)
		kernel_panic("FATAL: kmalloc not yet initialized\n");

	/* correct padding */
	size = size % HEAP_PADDING ?
		size + (HEAP_PADDING - size % HEAP_PADDING) : size;

	return _kmalloc_fast(size, _get_first_bit_set(size));
}

/**
 * Allocate n number of elements of size size.
 * The memory is set to 0. Returns NULL on failure.
 */
void *kcalloc(size_t n, size_t size)
{
	void *rval = NULL;

	if (!size || !n)
		return NULL;

	size *= n;
	/* correct padding */
	size = size % HEAP_PADDING ? 
		size + (HEAP_PADDING - size % HEAP_PADDING) : size;
	rval = _kmalloc_fast(size, _get_first_bit_set(size));

	if (!rval)
		return NULL;

	memset(rval, 0, size);
	return rval;
}

/**
 * Resize allocated memory pointed by ptr to size bytes.
 * If ptr is NULL, then krealloc acts like kmalloc(size).
 * If size is 0, then krealloc acts like kfree(ptr) and
 * returns NULL.
 * Normally krealloc returns a pointer to new memory or
 * NULL on failure. Original data is unmodified and new
 * memory isn't initialized.
 */
void *krealloc(void *ptr, size_t size)
{
	size_t orig_size = size; /* store original size */
	size_t fsize, fsize_index; /* free block size and size index */
	void *rval;
	void *orig_val = ptr; /* if we have to allocate a new block */
	memblock_ptr t_block;
	memblock_ptr bp = NULL; /* just to be shure */
	
	if (size && !ptr)
	{
		return kmalloc(size);
	} else if (!size && ptr)
	{
		kfree(ptr);
		return NULL;
	} else if (!size && !ptr)
	{
		return NULL; /* ptr = NULL and size = 0 */
	}

	bp   = (memblock_ptr) (ptr - sizeof(usedblock_t));
	size = size % HEAP_PADDING ? 
		size + (HEAP_PADDING - size % HEAP_PADDING) : size;

	/* if next block is free and it is large enough
	 *	allocate new memory from next block
	 *	add reminder to correct free list or reserve whole
	 *	return old pointer
	 */
	if ((void *) bp->used.next->first > (void *) bp->used.next)
	{
		fsize  = (void *) bp->used.next->free.next - (void *) bp;
		fsize -= sizeof(usedblock_t);
		if (fsize < size)
			goto krealloc_allocnew;
	} else
	{
		goto krealloc_allocnew; /* this should be in a function? */
	}

	/* calculate reminder free mem */
	fsize  = (void *) bp->used.next->free.next - (void *) bp;
	fsize -= sizeof(usedblock_t) - size - sizeof(usedblock_t);
	
	t_block = fsize < FREEMEM_MIN ? NULL :
		(memblock_ptr) ((void *) bp + sizeof(usedblock_t) + size);

	if (t_block)
	{
		/* unlink the next free block from list */
		bp->used.next->free.next_free->free.prev_free = bp->used.next->free.prev_free;
		bp->used.next->free.prev_free->free.next_free = bp->used.next->free.next_free;

		/* create a new free block after the reserved area */
		t_block->free.next = bp->used.next->free.next;
		bp->used.next->free.next->used.prev = t_block;
		t_block->free.prev = bp;
		bp->used.next = t_block;

		fsize_index = _get_first_bit_set(fsize);
		/* link new free block to appropriate free list */
		t_block->free.prev_free = (memblock_ptr) &freemem_bp[fsize_index];
		t_block->free.next_free = freemem_bp[fsize_index].next_free;
		t_block->free.prev_free->free.next_free = t_block;
		t_block->free.next_free->free.prev_free = t_block;
	} else
	{
		t_block = bp->used.next;
		/* unlink from free list */
		t_block->free.next_free->free.prev_free = t_block->free.prev_free;
		t_block->free.prev_free->free.next_free = t_block->free.next_free;
		
		/* update correct pointers to next block */
		bp->used.next->free.next->used.prev = bp;
		bp->used.next = bp->used.next->free.next;
	}

	/* original void *ptr is valid */
	return ptr;
	
	/* if the the next block isn't free or it is too small
	 * 	allocate a new block large enough
	 * 	copy data to new block
	 * 	free old block and return new block
	 * 	on error return NULL
	 */
krealloc_allocnew:
	rval = _kmalloc_fast(size, _get_first_bit_set(size));

	if (!rval)
		return NULL;

	memcpy(rval, orig_val, orig_size);
	kfree(orig_val);
	return rval;
}

/**
 * Free the memory reserved through ptr. Don't fail if user is
 * trying to release a NULL pointer. This will fail though if
 * given invalid pointer - it will cause undefined behaviour.
 * 
 * @param void *ptr
 * @return none
 */
void kfree(void *ptr)
{
	int size_index;
	size_t size;

	if (!ptr)
		return;
	
	memblock_ptr bp = (memblock_ptr) (ptr - sizeof(usedblock_t));
	memblock_ptr prev = bp->used.prev, next = bp->used.next;
	
	/* check the status of neighbor blocks
	 * if a memblock_ptr's element first is larger
	 * than the memblock_ptr itself, it is a free block */
	if ((void *) prev->first > (void *) prev)
	{
		if ((void *) next->first > (void *) next)
		{
			/* unlink 'prev' and 'next' from free block list */
			prev->free.next_free->free.prev_free = prev->free.prev_free;
			prev->free.prev_free->free.next_free = prev->free.next_free;
			next->free.next_free->free.prev_free = next->free.prev_free;
			next->free.prev_free->free.next_free = next->free.next_free;
			/* merge all 3 together and
			 * link to appropriate free block list */
			prev->free.next            = next->free.next;
			prev->free.next->used.prev = prev;
			
			size = (void *) prev->free.next - (void *) prev - sizeof(usedblock_t);
			size_index = _get_first_bit_set(size);

			/* link to list of free blocks */
			prev->free.next_free = freemem_bp[size_index].next_free;
			prev->free.next_free->free.prev_free = prev;
			prev->free.prev_free = (memblock_ptr) &freemem_bp[size_index];
			freemem_bp[size_index].next_free = prev;
		} else
		{
			/* 'prev' block is free, merge
			 * unlink 'prev' from free block list */
			prev->free.next_free->free.prev_free = prev->free.prev_free;
			prev->free.prev_free->free.next_free = prev->free.next_free;

			prev->free.next = next;
			prev->free.next->used.prev = prev;

			size = (void *) prev->free.next - (void *) prev - sizeof(usedblock_t);
			size_index = _get_first_bit_set(size);

			/* link to list of free blocks */
			prev->free.next_free = freemem_bp[size_index].next_free;
			prev->free.next_free->free.prev_free = prev;
			prev->free.prev_free = (memblock_ptr) &freemem_bp[size_index];
			freemem_bp[size_index].next_free = prev;
		}
	} else if ((void *) next->first > (void *) next)
	{
		/* the 'next' block is free, merge
		 * unlink 'next' from free block list */
		next->free.next_free->free.prev_free = next->free.prev_free;
		next->free.prev_free->free.next_free = next->free.next_free;

		bp->free.next = next->free.next;
		bp->free.prev = prev;
		bp->free.next->used.prev = bp;

		size = (void *) bp->free.next - (void *) bp - sizeof(usedblock_t);
		size_index = _get_first_bit_set(size);

		/* link to list of free blocks */
		bp->free.next_free = freemem_bp[size_index].next_free;
		bp->free.next_free->free.prev_free = bp;
		bp->free.prev_free = (memblock_ptr) &freemem_bp[size_index];
		freemem_bp[size_index].next_free = bp;
	} else {
		bp->free.next = next;
		bp->free.prev = prev;

		size = (void *) bp->free.next - (void *) bp - sizeof(usedblock_t);
		size_index = _get_first_bit_set(size);

		/* link to list of free blocks */
		bp->free.next_free = freemem_bp[size_index].next_free;
		bp->free.next_free->free.prev_free = bp;
		bp->free.prev_free = (memblock_ptr) &freemem_bp[size_index];
		freemem_bp[size_index].next_free = bp;
	}
}

/**
 * Initialize the heap to contain basic structures.
 * returns -1 on failure, 0 on success
 * 
 * NOTE: The structure of the heap should always be so
 * that there is a used block after a free block. At
 * the moment the code tries to honour this, but if you
 * add something, then remember to honour this behaviour!
 */
int kmalloc_init(void *ptr, size_t size)
{
	int i, size_index;
	memblock_ptr bp1, bp2, bp3;

	/* NULL or 0? */
	if (!ptr || !size)
		return -1;

	/* is there enough space for basic structure? */
	if (size < sizeof(usedblock_t) * 2 + sizeof(freeblock_t))
		return -1;

	/* correct padding */
	size -= size % HEAP_PADDING;

	for (i = 0; i < FREEMEM_LISTS; i++)
	{
		freemem_bp[i].next = freemem_bp[i].prev = NULL;
		freemem_bp[i].next_free = freemem_bp[i].prev_free = (memblock_ptr) &freemem_bp[i];
	}

	bp1 = (memblock_ptr) ptr;
	bp2 = (memblock_ptr) (ptr + sizeof(usedblock_t));
	bp3 = (memblock_ptr) (ptr + size - sizeof(usedblock_t));
	bp1->used.prev = bp1;	/* MARKER! */
	bp1->used.next = bp2;
	bp2->free.prev = bp1;
	bp2->free.next = bp3;
	bp3->used.prev = bp2;
	bp3->used.next = bp3;	/* MARKER! */

	size_index = _get_first_bit_set(size);

	bp2->free.prev_free = (memblock_ptr) &freemem_bp[size_index];
	bp2->free.next_free = freemem_bp[size_index].next_free;
	bp2->free.prev_free->free.next_free = bp2;
	bp2->free.next_free->free.prev_free = bp2;

	kmalloc_initialized = 1;
	return 0;
}
