#include "kernel/arch/current/halt.hh"
#include "kernel/arch/current/stacktrace.hh"
#include <string.h>
#include <iostream>

void getStackTrace(void *traceptrs[], size_t n_traceptrs, void *baseptr) {
	memset(traceptrs, 0, sizeof(void*) * n_traceptrs);
	
	uintptr_t *framePointerPtr = (uintptr_t *)getFramePointer();
	uintptr_t *maxFramePointer = framePointerPtr + 4096;
	void *sanity = (void*)*(framePointerPtr + 2);
	

	if (sanity != (void*)traceptrs) {
		kout << "getStackTrace misbehaving, halting" << endl;
		halt();
	}

	size_t i = 0;
	while ((i < n_traceptrs) && (framePointerPtr < maxFramePointer)) {
		traceptrs[i] = (void*)*(framePointerPtr + 1);
		uintptr_t *newFramePointer = (uintptr_t*)*framePointerPtr;

		// sanity check
		if (newFramePointer < framePointerPtr) {
			break;
		}
		framePointerPtr = newFramePointer;
		i ++;
	}
}
