#ifndef __REFCOUNT_INCLUDED__
#define __REFCOUNT_INCLUDED__

#include <kernel/atomic.hh>

class RefCount : private Atomic {
public:
	RefCount() : Atomic(0) {
	}

	inline operator int32_t() const {
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

	inline bool operator --() {
		return decAndIsZero();
	}

	inline bool release() {
		return -- *this;
	}

	inline void clear() {
		this->Atomic::operator=(0);
	}

	inline void set1() {
		this->Atomic::operator=(1);
	}

	inline void operator=(int32_t value) {
		this->Atomic::operator=(value);
	}
};

#endif
