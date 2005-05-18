#include "../../intoa.c"

void printout(int n, char *e, char *actual) 
{
        printf("Test %d: \texpected \"%s\"\n", n, e);
        printf("         \tgot      \"%s\"\n", actual);
}

int main() 
{
	char buf[256];
	int errors = 0;	

	snprintf(buf, 64, "%f%%%sA%cA%cA%cAAAA%sAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", 
		"BBBBBBBBBBB", 'C', 'D', 'E',
		"YAAAAAAABBBBCCCCCCCCDDDDDDDDEEEEEEEEFFFFFFFFGGGGGGGGGHHHHHHHHIIIIIIJJJJJJ");

	errors += (strncmp(buf, 
	      // 012345678901234567890123456789012345678901234567890123456789012
		"f%BBBBBBBBBBBACADAEAAAAYAAAAAAABBBBCCCCCCCCDDDDDDDDEEEEEEEEFFFF",
		70) != 0);
	printout(1, "f%BBBBBBBBBBBACADAEAAAAYAAAAAAABBBBCCCCCCCCDDDDDDDDEEEEEEEEFFFF",
		buf);
	
	snprintf(buf, 40, "%b", 0xF0F0F0F0);
	errors += (strcmp(buf, "11110000111100001111000011110000") != 0);
	printout(2, "11110000111100001111000011110000", buf);

	snprintf(buf, 40, "%o", 0123456712);
	errors += (strcmp(buf, "123456712") != 0);
	printout(3, "123456712", buf);

	printf("%d out of 3 tests passed\n", 3 - errors);

	return errors;
}
