#ifndef __STDDEF_H__
#define __STDDEF_H__

#include <inttypes.h>
#include <stdarg.h>
#include_next <stddef.h>

#ifdef __cplusplus

# define NULL 0

#else

# define NULL ((void *) 0)
  typedef  int32_t wchar_t;

#endif

#ifdef __arm__
typedef unsigned int size_t;
typedef          int ssize_t;
#else
typedef unsigned long int size_t;
typedef          long int ssize_t;
#endif
typedef  int32_t ptrdiff_t;
typedef uint32_t wint_t;

#endif
