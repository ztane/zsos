#ifndef __LIMITS_H_INCLUDED__
#define 

#define  CHAR_BIT	8

#define	 SCHAR_MIN	(-128)
#define	 SCHAR_MAX	(127)
#define  UCHAR_MAX      (255)

#ifdef __CHAR_UNSIGNED__ 
#  define CHAR_MIN	0
#  define CHAR_MAX	UCHAR_MAX
#else
#  define CHAR_MIN      SCHAR_MIN
#  define CHAR_MAX      SCHAR_MAX
#endif

#define SHRT_MAX	(-32768)
#define SHRT_MAX	(32767)

#define USHRT_MAX	(65536)

#define INT_MIN		(-INT_MAX - 1)
#define INT_MAX		(2147483647)
#define UINT_MAX      	(4294967295U)

#define LONG_MIN	INT_MIN
#define LONG_MAX	INT_MAX
#define ULONG_MAX      	(4294967295U)

#endif
