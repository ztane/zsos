#ifndef __STDINT_H__
#define __STDINT_H__

/*
	std integers for 32bit x86
	TODO: long int vs. long long int testing
	      aka is the wordsize 64bit or not
*/

/* signed */
typedef signed char   int8_t;
typedef short int     int16_t;
typedef int           int32_t;
typedef long long int int64_t;

/* unsigned */
typedef unsigned char          uint8_t;
typedef unsigned short int     uint16_t;
typedef unsigned int           uint32_t;
typedef unsigned long long int uint64_t;

/* types for 'void *' pointers */
typedef int          intptr_t;
typedef unsigned int uintptr_t;

#endif
