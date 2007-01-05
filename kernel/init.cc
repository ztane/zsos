#include "init.hh"


namespace init {

static int nfunc = 0;
static Init *list[8];

void setup(Init *ptr)
{
	if (nfunc < 8) {
		list[nfunc] = ptr;
		nfunc ++;
	}
}

void run()
{
	for (int i = 0; i < 8; i ++)
		if (list[i])
			(list[i])->init();
}

};
