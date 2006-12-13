volatile int argc = 5;
char *argv[] = { "a", "b", "c", "d", "e" };

int main(int argc, char *argv[]) {
	argc ++;
	*argv ++;
}

int volatile __CRT_startup() {
	main(argc, argv);
}
