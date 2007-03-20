#include <atomic.hh>
#include <iostream>
#include <bottomhalves.hh>

Atomic __bh_active_flag;
static BHFUNC funcs[32];
extern "C" {
	void __execute_bottom_halves() {
		while (__bh_active_flag) {
			for (int i = 0; i < 32; i++) {
				if (__bh_active_flag & (1 << i) 
					&& funcs[i]) 
				{
					__bh_active_flag &= ~ (1 << i);
					funcs[i]();

					// in case we got more bottom halves...
					break;
				}
			}
		}
	}
};

void markBottomHalf(int number) {
	kout << "MARK BOTTOM HALF" << endl;
	__bh_active_flag |= 1 << number;
}

void registerBottomHalf(int number, BHFUNC func) {
	funcs[number] = func;
}

void unregisterBottomHalf(int number) {
	funcs[number] = 0;
}

