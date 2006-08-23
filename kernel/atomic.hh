#ifndef __ATOMIC_INCLUDED__
#define __ATOMIC_INCLUDED__

#include <inttypes.h>
#define ____LOCK ""

class Atomic {
private:
	int32_t value;
public:
	inline Atomic() {
		value = 0;
	}

	inline Atomic(int32_t val) {
		value = val;
	}

	inline operator int32_t() const {
		int32_t rv;
		__asm__ __volatile__(
			____LOCK "movl %1, %0"
			:"=m"(rv):"m"(value)
		);
		return rv;
	}

	inline void operator=(int32_t val) {
		__asm__ __volatile__(
			____LOCK "movl %1, %0"
			:"=m"(value):"m"(val)
		);
	}

	inline void operator=(const Atomic& val) {
		*this = val.value;
	}

	inline void operator += (int32_t val) {
		__asm__ __volatile__(
			____LOCK "addl %1, %0"
			:"=m"(value):"m"(val), "m"(value));
	}	

	inline void operator -= (int32_t val) {
		__asm__ __volatile__(
			____LOCK "subl %1, %0"
			:"=m"(value):"m"(val), "m"(value));
	}

	inline void operator ++() {
		__asm__ __volatile__(
			____LOCK "incl %0"
			:"=m"(value) : "m"(value));
	}

	inline void operator ++(int) {
		++ *this;
	}

	inline void operator --() {
		__asm__ __volatile__(
			____LOCK "decl %0"
			:"=m"(value) : "m"(value));
	}

	inline void operator --(int) {
		-- *this;
	}

	inline bool dec_and_test() {
		char rv;
		__asm__ __volatile__ (
			____LOCK "decl %0;"
			"sete %1"
			:"=m"(value), "=r"(rv) : "m"(value));

		return rv != 0;
	}

	inline bool inc_and_test() {
		char rv;
		__asm__ __volatile__ (
			____LOCK "incl %0;"
			"sete %1"
			:"=m"(value), "=r"(rv): "m"(value));

		return rv != 0;
	}

	inline bool sub_and_test(int32_t val) {
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
