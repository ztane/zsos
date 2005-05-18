int main(int argc, char *argv[]) 
{
	char buf[256];
	char buf2[sizeof(buf)];

	if (strlen("") != 0) 
	{
		printf("strlen 1 failed\n");
		return 1;
	}
	if (strlen("0123456789") != 10) 
	{
		printf("strlen 2 failed\n");
		return 1;
	}

	strncpy(buf, "0123456789012345678901234567890", 6);
	strncpy(buf + 5, "ABC", 8);
	if (strcmp(buf, "01234ABC") != 0)
	{
		printf("strncpy failed\n");
		printf("result: %s\n", buf);
		return 1;
	}
	
	memset(buf, 0, sizeof(buf));
	memset(buf, 'A', 64);
	memset(buf + 10, 'B', 23);
	memset(buf + 15, 'C', 5);

	if (strcmp("AAAAAAAAAABBBBBCCCCCBBBBBBBBBBBBBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
		buf) != 0)
		return 1;

	memcpy(buf2, buf, sizeof(buf2));
	return strcmp(buf, buf2) != 0;
}

