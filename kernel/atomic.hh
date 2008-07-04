#ifndef __ATOMIC_INCLUDED__
#define __ATOMIC_INCLUDED__

#include <inttypes.h>
#define ____LOCK ""

class Atomic {
private:
	volatile int32_t value;
public:
	inline Atomic() {
		value = 0;
	}

	inline Atomic(int32_t val) {
		value = val;
	}

	inline operator int32_t() const volatile {
		int32_t rv;
		__asm__ __volatile__(
			____LOCK "movl %1, %0"
			:"=r"(rv):"m"(value)
		);
		return rv;
	}

	inline void operator=(int32_t val) volatile {
		__asm__ __volatile__(
			____LOCK "movl %1, %0"
			:"=m"(value):"r"(val)
		);
	}

	inline void operator=(const Atomic& val) volatile {
		*this = val.value;
	}

	inline void operator += (int32_t val) volatile {
		__asm__ __volatile__(
			____LOCK "addl %1, %0"
			:"=m"(value):"r"(val), "m"(value));
	}	

	inline void operator -= (int32_t val) volatile {
		__asm__ __volatile__(
			____LOCK "subl %1, %0"
			:"=m"(value):"r"(val), "m"(value));
	}

	inline void operator &= (int32_t val) volatile {
		__asm__ __volatile__(
			____LOCK "andl %1, %0"
			:"=m"(value): "r"(val), "m"(value));
	}

	inline void operator |= (int32_t val) volatile {
		__asm__ __volatile__(
			____LOCK "orl %1, %0"
			:"=m"(value):"r"(val), "m"(value));
	}

	inline void operator ^= (int32_t val) volatile {
		__asm__ __volatile__(
			____LOCK "xorl %1, %0"
			:"=m"(value):"r"(val), "m"(value));
	}

	inline void operator ++() volatile {
		__asm__ __volatile__(
			____LOCK "incl %0"
			:"=m"(value) : "m"(value));
	}

	inline void operator ++(int) volatile {
		++ *this;
	}

	inline void operator --() volatile {
		__asm__ __volatile__(
			____LOCK "decl %0"
			:"=m"(value) : "m"(value));
	}

	inline void operator --(int) volatile {
		-- *this;
	}

	inline bool decAndIsZero() volatile {
		char rv;
		__asm__ __volatile__ (
			____LOCK "decl %0;"
			"sete %1"
			:"=m"(value), "=r"(rv) : "m"(value));

		return rv != 0;
	}

	inline bool incAndIsZero() volatile {
		char rv;
		__asm__ __volatile__ (
			____LOCK "incl %0;"
			"sete %1"
			:"=m"(value), "=r"(rv): "m"(value));

		return rv != 0;
	}

	inline bool subAndIsZero(int32_t val) volatile {
		char rv;
		__asm__ __volatile__ (
			____LOCK "addl %2, %0;"
			"sete %1"
			:"=m"(value), "=r"(rv) : "r"(val));

		return rv != 0;
	}
};

#undef ____LOCK

#endif
