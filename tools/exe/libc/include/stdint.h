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

/* types for FAST integer types */
typedef signed char   		int_fast8_t;
typedef int     		int_fast16_t; /* (sic) */
typedef int         		int_fast32_t;
typedef long long int		int_fast64_t;

typedef unsigned char           uint_fast8_t;
typedef unsigned int            uint_fast16_t; /* (sic) */
typedef unsigned int            uint_fast32_t; 
typedef unsigned long long int  uint_fast64_t;

/* types for least integer types */
typedef signed char   		int_least8_t;
typedef short int     		int_least16_t; 
typedef int         		int_least32_t;
typedef long long int		int_least64_t;

typedef unsigned char           uint_least8_t;
typedef unsigned short int      uint_least16_t; 
typedef unsigned int            uint_least32_t; 
typedef unsigned long long int  uint_least64_t;

/* Maximum width types */
typedef long long int      	intmax_t;
typedef unsigned long long int 	uintmax_t;

#define INT8_MIN 	(-128) 
#define INT8_MAX 	127 
#define UINT8_MAX 	0xFF

#define INT16_MIN 	(-32768) 
#define INT16_MAX 	32767 
#define UINT16_MAX 	0xFFFF

#define INT32_MIN 	((int)(0x80000000)) 
#define INT32_MAX 	      (0x7FFFFFFF)
#define UINT32_MAX 	      (0xFFFFFFFF)

#define INT64_MIN 	((long long int)(0x8000000000000000LL)) 
#define INT64_MAX 	  	        (0x7FFFFFFFFFFFFFFFLL)
#define UINT64_MAX 	      		(0xFFFFFFFFFFFFFFFFULL)



#define INT_LEAST8_MIN 		(-128) 
#define INT_LEAST8_MAX 		127 
#define UINT_LEAST8_MAX 	0xFF

#define INT_LEAST16_MIN 	(-32768) 
#define INT_LEAST16_MAX 	32767 
#define UINT_LEAST16_MAX 	0xFFFF

#define INT_LEAST32_MIN 	((int)(0x80000000)) 
#define INT_LEAST32_MAX 	      (0x7FFFFFFF)
#define UINT_LEAST32_MAX 	      (0xFFFFFFFF)

#define INT_LEAST64_MIN 	((long long int)(0x8000000000000000LL)) 
#define INT_LEAST64_MAX 	  	        (0x7FFFFFFFFFFFFFFFLL)
#define UINT_LEAST64_MAX 	      		(0xFFFFFFFFFFFFFFFFULL)



#define INT_FAST8_MIN 		(-128) 
#define INT_FAST8_MAX 		127 
#define UINT_FAST8_MAX 		0xFF

#define INT_FAST16_MIN 		(-32768) 
#define INT_FAST16_MAX 		32767 
#define UINT_FAST16_MAX 	0xFFFF

#define INT_FAST32_MIN 		((int)(0x80000000)) 
#define INT_FAST32_MAX 	      	(0x7FFFFFFF)
#define UINT_FAST32_MAX 	(0xFFFFFFFF)

#define INT_FAST64_MIN 		((long long int)(0x8000000000000000LL)) 
#define INT_FAST64_MAX 	  	(0x7FFFFFFFFFFFFFFFLL)
#define UINT_FAST64_MAX 	(0xFFFFFFFFFFFFFFFFULL)



#define INTPTR_MIN 	((int)(0x80000000)) 
#define INTPTR_MAX 	      (0x7FFFFFFF)
#define UINTPTR_MAX 	      (0xFFFFFFFF)

#define INTMAX_MIN 	((long long int)(0x8000000000000000LL)) 
#define INTMAX_MAX 	  	        (0x7FFFFFFFFFFFFFFFLL)
#define UINTMAX_MAX 	      		(0xFFFFFFFFFFFFFFFFULL)


#endif
