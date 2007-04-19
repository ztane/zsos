int x;
volatile int argc = 5;
char *argv[] = { "a", "b", "c", "d", "e" };

void chug(register beers)
{
	char howmany[8], *s;

	s = beers != 1 ? "s" : "";
	printf("%d bottle%s of beer on the wall,\n", beers, s);
	printf("%d bottle%s of beeeeer . . . ,\n", beers, s);
	printf("Take one down, pass it around,\n");

	if(--beers) sprintf(howmany, "%d", beers); else strcpy(howmany, "No more");
		s = beers != 1 ? "s" : "";
		printf("%s bottle%s of beer on the wall.\n", howmany, s);
	}
}

int main(int argc, char *argv[]) {
	register beers;

	for(beers = MAXBEER; beers; chug(beers--))
  		puts("");

	puts("\nTime to buy more beer!\n");

	exit(0);
}

