#ifndef __REFCOUNT_INCLUDED__
#define __REFCOUNT_INCLUDED__

#include <kernel/atomic.hh>

class RefCount : private Atomic {
public:
	RefCount() : Atomic(0) {		
	}

	inline operator int32_t() {
		return Atomic::operator int32_t();
	}

	inline void operator ++() {
		Atomic::operator ++();
	}

	inline void operator ++(int) {
		++ *this;
	} 

	inline void acquire() {
		++ *this;
	}

	inline bool operator --() 
	{
		return !dec_and_test();
	}

	inline bool release() {
		return -- *this;
	}
};

#endif 
