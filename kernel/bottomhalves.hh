#ifndef __BOTTOMHALVES_HH__
#define __BOTTOMHALVES_HH__

const int BH_SCHEDULER	=	1;
const int BH_TIMER	=	0;

typedef void (*BHFUNC)();

void markBottomHalf(int number);
void registerBottomHalf(int number, BHFUNC func);
void unregisterBottomHalf(int number);

#endif
