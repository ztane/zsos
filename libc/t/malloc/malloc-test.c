#include <stdio.h>
#include <stdlib.h>
#include "../../malloc.h"

#define TEST_STRING_1 "abcdefghijklmnopqrstuvwxyz0123"
#define TEST_STRING_2 "ABCDEFGHIJKLMNOPQRSTUVWZYX0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

static char heap_mem[1048576]; /* used for testing purposes */

int main(void)
{
	char *foo, *bar, *tmp;
	
	if (kmalloc_init(heap_mem, sizeof(heap_mem)) < 0)
	{
		printf("kmalloc_init() returned < 0: some error\n");
		return 1;
	}

	
	foo = kmalloc(strlen(TEST_STRING_1) + 1);
	if (foo == NULL)
	{
		printf("kmalloc() returned a NULL pointer\n");
		return 1;
	}
	strcpy(foo, TEST_STRING_1);
	if (strcmp(foo, TEST_STRING_1) != 0)
	{
		printf("kmalloc() failed\n");
		return 1;
	}
	
	bar = kmalloc(strlen(TEST_STRING_1) + 1);
	if (bar == NULL)
	{
		printf("kmalloc() returned a NULL pointer\n");
		return 1;
	}
	strcpy(bar, TEST_STRING_1);
	if (strcmp(bar, TEST_STRING_1) != 0)
	{
		printf("kmalloc() failed\n");
		return 1;
	}

	printf("%s\n%s\n", foo, bar);


	tmp = krealloc(foo, strlen(TEST_STRING_2) + 1);
	if (tmp == NULL)
	{
		printf("krealloc() returned a NULL pointer\n");
		return 1;
	}
	foo = tmp;
	strcpy(foo, TEST_STRING_2);
	
	printf("%s\n", foo);
	
	tmp = krealloc(foo, 0); /* works like free */
	if (tmp != NULL)
	{
		printf("krealloc(void *, 0) didn't return NULL\n");
		return 1;
	}
	kfree(bar);

	return 0;
}


