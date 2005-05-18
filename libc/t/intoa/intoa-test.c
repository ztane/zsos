#include <limits.h>
#include "stdio.h"

int test(int n, char *buf, size_t bufsize, int value, int exp1, const char *exp2) 
{
	int rv;
	rv   = (intoa(buf, bufsize, value) != exp1);
	rv  += (strcmp(buf, exp2) != 0);

	printf("Test %d: \texpected \"%s\"\n", n, buf);
	printf("         \tgot      \"%s\"\n", exp2);
	return rv;
}

int main(int argc, char *argv[])
{
	char buf[12];
	int rv      = 0;
	int n_tests = 0;

	//         #   buf  size          val      expected
	rv += test(1,  buf, sizeof(buf),  0,       0,  "0"          ); n_tests ++;
	rv += test(2,  buf, sizeof(buf), -12345,   0,  "-12345"     ); n_tests ++;
	rv += test(3,  buf, sizeof(buf),  12345,   0,  "12345"      ); n_tests ++;
	rv += test(4,  buf, sizeof(buf),  INT_MIN, 0,  "-2147483648"); n_tests ++;
	rv += test(5,  buf, sizeof(buf),  INT_MAX, 0,  "2147483647" ); n_tests ++;
	rv += test(6,  buf, 5,            INT_MAX, -1, "2147"       ); n_tests ++;
	rv += test(7,  buf, 5,            INT_MIN, -1, "-214"       ); n_tests ++;

	printf("%d out of %d tests passed\n", n_tests - rv, n_tests);
	return rv;
}
